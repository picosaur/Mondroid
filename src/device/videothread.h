#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H
#include <QMutex>
#include <QPointer>
#include <QThread>
#include "device/adbclient.h"

class AdbClient;
class AdbDeviceInfo;

class VideoThread : public QThread
{
	Q_OBJECT

public:
    enum ImageFormat { ImageRaw, ImageJpg, ImagePng };

    VideoThread(QObject *parent = nullptr);
    virtual ~VideoThread();

    void setHost(const QString &host, int port);
    void setDevice(const QString &deviceId);
    void setImageFormat(ImageFormat mode);
    void setImageRate(double fps);

    void setImageScale(double scale);
    void setImageScalePercent(int p);
    int getScaledSize(int value) const;

signals:
    void imageReady(const QImage &image);

protected:
    AdbClient *adb() const;
    const DeviceInfo &devInfo() const;

private:
    virtual void run();
    virtual void loop();

    QString m_host{"127.0.0.1"};
    int m_port{5037};
    QString m_deviceId{};
    ImageFormat m_imageFormat{ImageRaw};
    double m_imageScale{1.0};
    unsigned long m_imageRateMs{100};

    QPointer<AdbClient> m_adb{};
    DeviceInfo m_devInfo{};
    mutable QMutex m_mutex;
};

#endif // VIDEOTHREAD_H
