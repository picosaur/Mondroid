// Based on DivvyDroid by Mladen Milinkovic <maxrd2@smoothware.net>
// https://github.com/maxrd2/DivvyDroid
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
    QSize phScreenSize{};
    QSize ovScreenSize{};
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
    QSize devStableScreenSize();
    QSize devPhysicalScreenSize();
    QSize devOverrideScreenSize();
    qint32 devScreenRotation();
    bool devIsScreenAwake();
    QList<QString> getDeviceList();

    bool connectToDevice();
    bool forwardTcpPort(int local, int remote);
    QImage fetchScreenRaw();
    QImage fetchScreenPng();
    QImage fetchScreenJpeg();
    bool startVideoStream(int timeLimit = 60);

    QByteArray inputTap(const QPoint &p);
    QByteArray inputSwipe(const QPoint &p1, const QPoint &p2, qint64 d);
    QByteArray inputKeyEvent(int ke);

    QByteArray shell(const QString &cmd);
    QByteArray sendToDevice(const QString &cmd);
    bool sendEvents(AdbEventList events);
    bool sendEvents(int deviceIndex, AdbEventList events);

    void connectToHost();
    void disconnectFromHost();
    void close();
    bool waitForConnected(int msecs = -1);
    bool waitForDisconnected(int msecs = -1);
    bool waitForReadyRead(int msecs = -1);
    QTcpSocket::SocketError error();
    qint64 bytesAvailable();
    qint64 isConnected();

    bool write(const void *data, qint64 max);
    bool write(const QByteArray &data);
    bool send(const QString &scmd);

    bool read(void *data, qint64 max);
    bool readStatus();
    QByteArray readResponse();
    QByteArray readAll();
    QByteArray readLine();
    QByteArray readAvailable();

    static QList<QString> getDeviceList(const QString &host, int port = 5037, int timeout = 3000);

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
