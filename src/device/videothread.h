#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H
#include <QThread>

class AdbClient;

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
    void setImageScale(double scale);
    void setImageScalePercent(double p);
    void setInterval(unsigned long ms);

signals:
    void imageReady(const QImage &image);

protected:
    void run();

private:
    void loop();

    QString m_host{"127.0.0.1"};
    int m_port{5037};
    QString m_deviceId{};
    ImageFormat m_imageFormat{ImageRaw};
    double m_imageScale{1.0};
    unsigned long m_interval{100};

    AdbClient *m_adb{};
    int m_imageWidth{};
};

#endif // VIDEOTHREAD_H
