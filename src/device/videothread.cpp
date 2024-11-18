#include "videothread.h"
#include <QImage>
#include "adbclient.h"

VideoThread::VideoThread(QObject *parent)
    : QThread(parent)
{}

VideoThread::~VideoThread()
{
	requestInterruption();
	wait();
}

void VideoThread::run()
{
    m_adb = new AdbClient();
    m_adb->setHost(m_host, m_port);
    m_adb->setDevice(m_deviceId);

    loop();

    m_adb->close();
    m_adb.clear();
}

void VideoThread::loop()
{
    while (!isInterruptionRequested()) {
        QImage img;
        if (m_adb->devIsScreenAwake()) {
            if (m_imageFormat == ImageRaw) {
                img = m_adb->fetchScreenRaw();
            } else if (m_imageFormat == ImageJpg) {
                img = m_adb->fetchScreenJpeg();
            } else if (m_imageFormat == ImagePng) {
                img = m_adb->fetchScreenPng();
            } else {
                return;
            }
        } else {
            img = QImage(320, 240, QImage::Format_RGB888);
            img.fill(Qt::black);
        }
        if (!img.isNull()) {
            emit imageReady(img.scaledToWidth(getScaledSize(img.width()), Qt::FastTransformation));
        }
        msleep(m_imageRateMs);
    }
}

void VideoThread::setHost(const QString &host, int port)
{
    m_host = host;
    m_port = port;
}

void VideoThread::setDevice(const QString &deviceId)
{
    m_deviceId = deviceId;
}

void VideoThread::setImageFormat(ImageFormat format)
{
    m_imageFormat = format;
}

void VideoThread::setImageRate(double fps)
{
    m_imageRateMs = 1000 / fps;
}

void VideoThread::setImageScale(double scale)
{
    QMutexLocker lk(&m_mutex);
    m_imageScale = scale;
}

void VideoThread::setImageScalePercent(int p)
{
    QMutexLocker lk(&m_mutex);
    m_imageScale = double(p) / 100.0;
}

int VideoThread::getScaledSize(int value) const
{
    QMutexLocker lk(&m_mutex);
    return int(double(value) * m_imageScale);
}

AdbClient *VideoThread::adb() const
{
    return m_adb;
}

const DeviceInfo &VideoThread::devInfo() const
{
    return m_devInfo;
}
