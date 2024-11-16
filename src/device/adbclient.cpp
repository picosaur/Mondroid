/*
   DivvyDroid - Application to screencast and remote control Android devices.

   Copyright (C) 2019 - Mladen Milinkovic <maxrd2@smoothware.net>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "adbclient.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QHostAddress>
#include <QPixmap>

AdbClient::AdbClient(QObject *parent)
	: QObject(parent)
{
	connect(&m_sock, &QTcpSocket::stateChanged, this, &AdbClient::stateChanged);
    connect(&m_sock, &QTcpSocket::errorOccurred, this, &AdbClient::errorOcurred);
    connect(&m_sock, &QTcpSocket::readyRead, this, &AdbClient::readyRead);
    connect(&m_sock, &QTcpSocket::bytesWritten, this, &AdbClient::bytesWritten);
}

AdbClient::~AdbClient()
{
	m_sock.close();
}

void AdbClient::setHost(const QString &host, int port)
{
    m_host = host;
    m_port = port;
}

void AdbClient::setDevice(const QString &deviceId)
{
    m_deviceId = deviceId;
}

DeviceInfo AdbClient::getDeviceInfo()
{
    DeviceInfo info;
    info.deviceId = m_deviceId;
    info.androidVer = devAndroidVer();
    info.isArch64 = devIsArch64();
    info.screenRotation = devScreenRotation();
    info.ovScreenSize = devOverrideScreenSize();
    info.phScreenSize = devPhysicalScreenSize();
    return info;
}

FramebufInfo AdbClient::getFramebufInfo()
{
    FramebufInfo fbInfo{};

    if (!connectToDevice()) {
        return {};
    }

    if (!send("framebuffer:")) {
        qWarning() << "FRAMEBUFFER unable to connect to framebuffer";
        return {};
    }
    if (!read(&fbInfo.version, sizeof(fbInfo.version))) {
        qDebug() << "FRAMEBUFFER error reading framebuffer version";
        return {};
    }

    bool res{};
    switch (fbInfo.version) {
    case 16: // version 0
        res = read(&fbInfo.v0, sizeof(fbInfo.v0));
        if (!write("0", 1)) {
            qDebug() << "FRAMEBUFFER error writing v0 request";
            return {};
        }
        break;
    case 1:
        res = read(&fbInfo.v1, sizeof(fbInfo.v1));
        Q_ASSERT(fbInfo.v1.size == fbInfo.v1.width * fbInfo.v1.height * fbInfo.v1.bpp / 8);
        break;
    case 2:
        res = read(&fbInfo.v2, sizeof(fbInfo.v2));
        break;
    default:
        res = {};
        break;
    }
    if (!res) {
        qDebug() << "FRAMEBUFFER error reading framebuffer info";
        return {};
    }

    fbInfo.valid = res;
    return fbInfo;
}

bool AdbClient::devIsArch64()
{
    const auto abi = shell("getprop ro.product.cpu.abi").simplified();
    return abi != "armeabi-v7a" && abi != "armeabi" && abi != "x86";
}

QString AdbClient::devAndroidVer()
{
    return AdbClient::shell("getprop ro.build.version.release").simplified();
}

QSize AdbClient::devStableScreenSize()
{
    auto res = shell("dumpsys display | grep -E 'StableDisplayWidth|StableDisplayHeight'");
    QRegExp re("\\b(StableDisplayWidth|StableDisplayHeight)=(\\d+)\\b");
    qint32 i{};
    i = re.indexIn(res, 0);
    qint32 w = i != -1 ? re.cap(2).toInt() : 0;
    i = re.indexIn(res, i + re.matchedLength());
    qint32 h = i != -1 ? re.cap(2).toInt() : 0;
    return {w, h};
}

QSize AdbClient::devPhysicalScreenSize()
{
    auto res = shell("wm size | grep Physical");
    res = res.split(':').last().simplified();
    auto arr = res.split('x');
    return {arr.first().toInt(), arr.last().toInt()};
}

QSize AdbClient::devOverrideScreenSize()
{
    auto res = shell("wm size | grep Override");
    res = res.split(':').last().simplified();
    auto arr = res.split('x');
    return {arr.first().toInt(), arr.last().toInt()};
}

int AdbClient::devScreenRotation()
{
    return (360 + shell("getprop ro.sf.hwrotation").simplified().toInt());
}

bool AdbClient::devIsScreenAwake()
{
    const auto res = shell("dumpsys input_method");
    int i = res.indexOf("mScreenOn=");
    if (i != -1) {
        i += 10;
    } else {
        i = res.indexOf("mInteractive=");
        if (i == -1) {
            qWarning() << "isScreenAwake() failed.";
            return true;
        }
        i += 13;
    }
    return res.at(i) == 't';
}

QList<QString> AdbClient::getDeviceList()
{
    if (!send("host:devices-l")) {
        return {};
    }
    QList<QString> list;
    const auto response{readResponse()};
    const auto devices{response.split('\n')};
    for (const auto &dev : devices) {
        auto info{dev.simplified()};
        if (info.isEmpty()) {
            continue;
        }
        const auto i{info.indexOf(' ')};
        auto deviceId{info.left(i)};
        if (deviceId.isEmpty()) {
            continue;
        }
        list.append(info.left(i));
    }
    return list;
}

bool AdbClient::connectToDevice()
{
    auto scmd{QString("host:transport:%1").arg(m_deviceId)};
    if (!send(scmd)) {
        qWarning() << __FUNCTION__ << "unable to connect to android device";
        return false;
    }
    return true;
}

bool AdbClient::forwardTcpPort(int local, int remote)
{
    auto scmd{QString("host-serial:%1:forward:%2;%3").arg(m_deviceId).arg(local).arg(remote)};
    if (!send(scmd)) {
        qWarning() << __FUNCTION__ << "unable to forward port to android device";
        return false;
    }
    return true;
}

QImage AdbClient::fetchScreenRaw()
{
    FramebufInfo fbInfo{};
    fbInfo = getFramebufInfo();

    if (!fbInfo.valid) {
        return {};
    }

    const int bytesPerLine = fbInfo.width() * fbInfo.bpp() / 8;
    QImage img(fbInfo.width(), fbInfo.height(), fbInfo.format());
    for (int y = 0, h = img.height(); y < h; y++) {
        if (!read(img.scanLine(y), bytesPerLine)) {
            qDebug() << __FUNCTION__ << "error reading framebuffer frame";
            return img;
        }
    }
    if (fbInfo.format() == QImage::Format_RGB32) {
        // swap R and B
        for (int y = 0, h = img.height(); y < h; y++) {
            uchar *s = img.scanLine(y);
            for (int x = 0, w = img.width(); x < w; x++) {
                s += 4;
                const int t = s[0];
                s[0] = s[2];
                s[2] = t;
            }
        }
    }
    readAll();
    return img;
}

QImage AdbClient::fetchScreenPng()
{
    if (!connectToDevice()) {
        return {};
    }
    if (!send("shell:stty raw; screencap -p")) {
        qWarning() << __FUNCTION__ << "error executing PNG screencap";
        return {};
    }
    return QImage::fromData(readAll());
}

QImage AdbClient::fetchScreenJpeg()
{
    if (!connectToDevice()) {
        return {};
    }
    if (!send("shell:stty raw; screencap -j")) {
        qWarning() << __FUNCTION__ << "error executing JPEG screencap";
        return {};
    }
    return QImage::fromData(readAll());
}

bool AdbClient::startVideoStream()
{
    if (!connectToDevice()) {
        return false;
    }
    if (!send("shell:stty raw; screenrecord --output-format=h264 -")) {
        qWarning() << __FUNCTION__ << "error executing screenrecord";
        return false;
    }
    return true;
}

QByteArray AdbClient::inputTap(const QPoint &p)
{
    return shell(QString("input tap %1 %2").arg(p.x()).arg(p.y()));
}

QByteArray AdbClient::inputSwipe(const QPoint &p1, const QPoint &p2, qint64 d)
{
    return shell(
        QString("input swipe %1 %2 %3 %4 %5").arg(p1.x()).arg(p1.y()).arg(p2.x()).arg(p2.y()).arg(d));
}

QByteArray AdbClient::inputKeyEvent(int ke)
{
    return shell(QString("input keyevent %1").arg(ke));
}

QByteArray AdbClient::shell(const QString &cmd)
{
    return sendToDevice(QString("shell:%1").arg(cmd));
}

QByteArray AdbClient::sendToDevice(const QString &cmd)
{
    if (!connectToDevice()) {
        return {};
    }
    if (!send(cmd)) {
        qWarning() << __FUNCTION__ << "unable to execute shell command:" << cmd;
        return {};
    }
    return readAll();
}

bool AdbClient::sendEvents(AdbEventList events)
{
    for (const AdbEvent &evt : events) {
        if (!write(&evt, sizeof(AdbEvent))) {
            qDebug() << __FUNCTION__ << "failed sending event";
            return false;
        }
    }
    return true;
}

bool AdbClient::sendEvents(int deviceIndex, AdbEventList events)
{
    AdbClient adb;
    if (!adb.connectToDevice()) {
        return false;
    }
    if (!adb.send(QString("dev:/dev/input/event%1").arg(deviceIndex))) {
        qDebug() << __FUNCTION__ << "failed opening device" << deviceIndex;
        return false;
    }
    return adb.sendEvents(events);
}

bool AdbClient::write(const void *data, qint64 max)
{
    int done = 0;
    while(max > done) {
        int n = m_sock.write((char*)data + done, max - done);
		if(n <= 0) {
			qDebug() << __FUNCTION__ << "failed";
            return false;
		}
        done += n;
    }
	return m_sock.waitForBytesWritten();
}

bool AdbClient::write(const QByteArray &data)
{
    return write(data.constData(), data.size());
}

bool AdbClient::send(const QString &scmd)
{
    connectToHost();
    write(QString("%1").arg(scmd.size(), 4, 16, QChar('0')).toLatin1());
    write(scmd.toLatin1());
    return readStatus();
}

bool AdbClient::read(void *data, qint64 max)
{
    int done = 0;
    while (max > done) {
        const int n = m_sock.read((char *) data + done, max - done);
        if (n < 0) {
            qDebug() << __FUNCTION__ << "failed";
            return false;
        }
        if (n == 0) {
            if (!m_sock.waitForReadyRead()) {
                return false;
            }
        }
        done += n;
    }
    return true;
}

QByteArray AdbClient::readAll()
{
	QByteArray buf;
    while (m_sock.waitForReadyRead()) {
        buf.append(m_sock.readAll());
    }
    Q_ASSERT(m_sock.state() == QAbstractSocket::UnconnectedState);
	return buf;
}

QByteArray AdbClient::readLine()
{
	while(!m_sock.canReadLine() && m_sock.waitForReadyRead());
	return m_sock.readLine();
}

QByteArray AdbClient::readAvailable()
{
	m_sock.waitForReadyRead();
	return m_sock.readAll();
}

bool AdbClient::readStatus()
{
	char buf[256];

	if(!read(buf, 4)) {
		qDebug() << __FUNCTION__ << "failed: protocol fault (no status)";
		return false;
	}

	if(memcmp(buf, "OKAY", 4) == 0) {
		return true;
	}

	if(memcmp(buf, "FAIL", 4)) {
		qDebug() << __FUNCTION__ << "failed: invalid status response";
		return false;
	}

	if(!read(buf, 4)) {
		qDebug() << __FUNCTION__ << "failed: missing error message length";
		return false;
	}

	buf[4] = 0;
	unsigned int len = strtoul((char*)buf, 0, 16);
	if(len > 255) len = 255;

	if(!read(buf, len)) {
		qDebug() << __FUNCTION__ << "failed: missing error message";
		return false;
	}
	buf[len] = 0;
	qDebug() << "ADB FAIL:" << buf;
	return false;
}

QByteArray AdbClient::readResponse()
{
	int len = 0;
	{
		char tmp[5];
		if(!read(tmp, 4)) {
			qDebug() << __FUNCTION__ << "failed: missing response length";
			return QByteArray();
		}
		tmp[4] = 0;
		len = strtoul((char*)tmp, 0, 16);
	}

    if (len > 0) {
        QByteArray res;
        res.resize(len + 1);
        res[0] = res[len] = 0;
        if (!read(res.data(), len)) {
            qDebug() << __FUNCTION__ << "failed: missing response data";
        }
        return res;
    }

    return QByteArray();
}

void AdbClient::connectToHost()
{
    auto sockHost = m_sock.property("m_host").toString();
    auto sockPort = m_sock.property("m_port").toInt();

    if (m_sock.state() == QTcpSocket::ConnectedState && sockHost == m_host && sockPort == m_port) {
        return;
    }

    //m_sock.setSocketOption(QTcpSocket::LowDelayOption, 1); // TCP_NODELAY
    m_sock.setSocketOption(QTcpSocket::KeepAliveOption, 1); // SO_KEEPALIVE
    m_sock.connectToHost(m_host, m_port, QIODevice::ReadWrite);
    if (!m_sock.waitForConnected()) {
        qWarning() << __FUNCTION__ << "failed: error connecting to adb server.";
    } else {
        m_sock.setProperty("m_host", m_host);
        m_sock.setProperty("m_port", m_port);
    }
}

void AdbClient::disconnectFromHost()
{
    return m_sock.disconnectFromHost();
}

void AdbClient::close()
{
    return m_sock.close();
}

bool AdbClient::waitForDisconnected(int msecs)
{
    return m_sock.waitForDisconnected(msecs);
}

bool AdbClient::waitForReadyRead(int msecs)
{
    return m_sock.waitForReadyRead(msecs);
}

QAbstractSocket::SocketError AdbClient::error()
{
    return m_sock.error();
}

qint64 AdbClient::bytesAvailable()
{
    return m_sock.bytesAvailable();
}

qint64 AdbClient::isConnected()
{
    return m_sock.state() != QTcpSocket::UnconnectedState;
}

QList<QString> AdbClient::getDeviceList(const QString &host, int port)
{
    const auto adb{new AdbClient()};
    adb->setHost(host, port);
    auto devList{adb->getDeviceList()};
    adb->deleteLater();
    return devList;
}
