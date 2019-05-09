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
#include "deviceinfo.h"

#include "adbclient.h"

#include <QProcess>
#include <QDebug>

DeviceInfo *aDev = nullptr;

DeviceInfo::DeviceInfo(const char *deviceId)
	: m_deviceId(deviceId),
	  m_inputTouch(-1),
	  m_inputPower(-1),
	  m_inputHome(-1),
	  m_inputVolume(-1)
{
}

bool
DeviceInfo::waitForDevice()
{
	QProcess shell;
	shell.start(QStringLiteral("adb"), QStringList() << QStringLiteral("wait-for-device"));
	shell.waitForFinished();
	if(shell.exitCode()) {
		qWarning() << "Failed to start adb server.";
		return false;
	}

	return true;
}

DeviceList
DeviceInfo::deviceList()
{
	AdbClient adb;
	if(!adb.send("host:devices-l"))
		return DeviceList();

	DeviceList devList;

	QList<QByteArray> devices = adb.readResponse().split('\n');
	for(const QByteArray &dev : devices) {
		QByteArray info = dev.simplified();
		if(info.isEmpty())
			continue;

		const int i = info.indexOf(' ');
		devList.insert(info.left(i), info.mid(i + 1));
	}

	return devList;
}

static bool
deviceIsWritable(int deviceNr)
{
	AdbClient adb;
	if(!adb.connectToDevice())
		return false;

	if(!adb.send(QByteArray("dev:").append(INPUT_DEV_PATH).append(QString::number(deviceNr))))
		return false;

	return true;
}

static bool
inputHasKey(const QVector<quint64> keyBits, quint64 keyCode)
{
	const int byte = keyCode / (aDev->isArch64() ? 64 : 32);
	const int bit = keyCode % (aDev->isArch64() ? 64 : 32);

	if(byte >= keyBits.size())
		return false;

	return (keyBits.at(byte) & (1ull << bit)) != 0;
}


void
DeviceInfo::connect(const char *deviceId)
{
	delete aDev;
	aDev = new DeviceInfo(deviceId);

	// device architecture
	aDev->m_arch64 = AdbClient::shell("file -L /system/bin/cat | grep 32-bit").size() == 0;

	// screen resolution
	const QByteArray res = AdbClient::shell("dumpsys display | grep -E 'mDisplayWidth|mDisplayHeight'").replace('\n', '\0');
	int i = res.indexOf("mDisplayWidth");
	aDev->m_screenWidth = i != -1 ? res.mid(res.indexOf('=', i) + 1).toInt() : 0;
	i = res.indexOf("DisplayHeight");
	aDev->m_screenHeight = i != -1 ? res.mid(res.indexOf('=', i) + 1).toInt() : 0;

	// enumerate input devices
	QList<QByteArray> inputs = AdbClient::shell("ls -d " INPUT_SYS_PATH "*").simplified().split(' ');

	for(const QByteArray &input : inputs) {
		int devIndex = input.mid(sizeof(INPUT_SYS_PATH) - 1).toInt();

		QList<QByteArray> res = AdbClient::shell(QByteArray("cat ")
							   .append(input).append("/name ")
							   .append(input).append("/capabilities/ev ")
							   .append(input).append("/capabilities/key ")
							   .append("2>/dev/null")).split('\n');
		if(res.size() < 3)
			continue;

		QString name = res.at(0).trimmed();
		int evBits = res.at(1).trimmed().toInt(nullptr, 16);
		QVector<quint64> keyBits;
		const QList<QByteArray> keyList = res.at(2).simplified().split(' ');
		for(auto it = keyList.crbegin(); it != keyList.crend(); ++it)
			keyBits.push_back(it->toULongLong(nullptr, 16));

		if(HAS_BIT(evBits, EV_SYN) && HAS_BIT(evBits, EV_ABS) && HAS_BIT(evBits, EV_KEY) && inputHasKey(keyBits, BTN_TOUCH)) {
			qDebug() << "INPUT device" << devIndex << name << "is touch screen";
			aDev->m_inputTouch = devIndex;
		} else if(HAS_BIT(evBits, EV_SYN) && HAS_BIT(evBits, EV_KEY)) {
			if(!deviceIsWritable(devIndex)) {
				qDebug() << "INPUT device" << devIndex << name << "is not writable";
				continue;
			}
			bool gotKey = false;
			if(inputHasKey(keyBits, KEY_HOMEPAGE)) {
				qDebug() << "INPUT device" << devIndex << name << "has home key";
				aDev->m_inputHome = devIndex;
				gotKey = true;
			}
			if(inputHasKey(keyBits, KEY_BACK)) {
				qDebug() << "INPUT device" << devIndex << name << "has back key";
				aDev->m_inputBack = devIndex;
				gotKey = true;
			}
			if(inputHasKey(keyBits, KEY_POWER)) {
				qDebug() << "INPUT device" << devIndex << name << "has power key";
				aDev->m_inputPower = devIndex;
				gotKey = true;
			}
			if(inputHasKey(keyBits, KEY_VOLUMEUP) && inputHasKey(keyBits, KEY_VOLUMEDOWN)) {
				qDebug() << "INPUT device" << devIndex << name << "has volume keys";
				aDev->m_inputVolume = devIndex;
				gotKey = true;
			}
			if(!gotKey)
				qDebug() << "INPUT device" << devIndex << name << "has some keys";
		} else {
			qDebug() << "INPUT device" << devIndex << name << "is not supported";
		}
	}
}

bool
DeviceInfo::isScreenAwake() const
{
	const QByteArray res = AdbClient::shell("service call power 12");
	return res.size() >= 32 && res.at(31) == '1';
}
