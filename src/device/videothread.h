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
    void setImageRate(double fps);

signals:
    void imageReady(const QImage &image);

protected:
    AdbClient *adb() const;
    int screenWidth() const;
    int screenHeight() const;
    int ovScreenWidth() const;
    int ovScreenHeight() const;
    int imageWidth() const;
    int imageHeight() const;

private:
    virtual void run();
    virtual void loop();

    QString m_host{"127.0.0.1"};
    int m_port{5037};
    QString m_deviceId{};
    ImageFormat m_imageFormat{ImageRaw};
    double m_imageScale{1.0};
    unsigned long m_imageRateMs{100};

    AdbClient *m_adb{};
    int m_screenWidth{};
    int m_screenHeight{};
    int m_ovScreenWidth{};
    int m_ovScreenHeight{};
    int m_imageWidth{};
    int m_imageHeight{};
};

#endif // VIDEOTHREAD_H
