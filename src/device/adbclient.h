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
#ifndef ADBCLIENT_H
#define ADBCLIENT_H

#include <QImage>
#include <QTcpSocket>
#include "fbinfo.h"

struct DeviceInfo
{
    QString deviceId{};
    QString androidVer{};
    bool isArch64{};
    int screenRotation{};
    int phScreenWidth{};
    int phScreenHeight{};
    int ovScreenWidth{};
    int ovScreenHeight{};
};

struct AdbEvent {
	AdbEvent(quint16 t, quint16 c = 0, qint32 v = 0)
		: time(0),
		  type(t),
		  code(c),
		  value(v)
	{}
    quint64 time{};
    quint16 type{};
    quint16 code{};
    qint32 value{};
};
typedef QList<AdbEvent> AdbEventList;

class AdbClient : public QObject
{
	Q_OBJECT

public:
	explicit AdbClient(QObject *parent = nullptr);
    ~AdbClient();

    void setHost(const QString &host, int port);
    void setDevice(const QString &deviceId);

    DeviceInfo getDeviceInfo();
    FramebufInfo getFramebufInfo();

    bool devIsArch64();
    QString devAndroidVer();
    QPair<int, int> devStableScreenSize();
    QPair<int, int> devPhysicalScreenSize();
    QPair<int, int> devOverrideScreenSize();
    qint32 devScreenRotation();
    bool devIsScreenAwake();
    QList<QString> getDeviceList();

    bool connectToDevice();
    bool forwardTcpPort(int local, int remote);
    QImage fetchScreenRaw();
    QImage fetchScreenPng();
    QImage fetchScreenJpeg();

    QByteArray shell(const char *cmd);
    bool sendEvents(AdbEventList events);
    bool sendEvents(int deviceIndex, AdbEventList events);

    void connectToHost();
    void disconnectFromHost();
    void close();
    bool waitForDisconnected(int msecs = -1);
    bool waitForReadyRead(int msecs = -1);
    QTcpSocket::SocketError error();
    qint64 bytesAvailable();
    qint64 isConnected();

    bool read(void *data, qint64 max);
    bool write(const void *data, qint64 max);
    bool write(const QByteArray &data);
    bool send(QByteArray command);
    bool readStatus();
    QByteArray readResponse();
    QByteArray readAll();
    QByteArray readLine();
    QByteArray readAvailable();

    static QList<QString> getDeviceList(const QString &host, int port = 5037);

signals:
	void stateChanged(QAbstractSocket::SocketState);
    void errorOcurred(QAbstractSocket::SocketError);
    void readyRead();
    void bytesWritten(qint64 bytes);

private:
    QString m_host{"127.0.0.1"};
    int m_port{5037};
    QString m_deviceId{};
    QTcpSocket m_sock{};
};

#endif // ADBCLIENT_H
