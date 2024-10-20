#include "videothread.h"
#include <QImage>
#include "adbclient.h"

VideoThread::VideoThread(QObject *parent)
    : QThread(parent)
{
}

VideoThread::~VideoThread()
{
	requestInterruption();
	wait();
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

void VideoThread::setImageScale(double scale)
{
    m_imageScale = scale;
}

void VideoThread::setImageScalePercent(double p)
{
    m_imageScale = p / 100.0;
}

void VideoThread::setInterval(unsigned long ms)
{
    m_interval = ms;
}

void VideoThread::loop()
{
    while (!isInterruptionRequested()) {
        QImage img;
        if (m_adb->devIsScreenAwake()) {
            if (m_imageFormat == ImageJpg) {
                img = m_adb->fetchScreenJpeg();
            } else if (m_imageFormat == ImageRaw) {
                img = m_adb->fetchScreenRaw();
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
            emit imageReady(img.scaledToWidth(m_imageWidth, Qt::FastTransformation));
        }
        msleep(500);
    }
}

void VideoThread::run()
{
    m_adb = new AdbClient();
    m_adb->setHost(m_host, m_port);
    m_adb->setDevice(m_deviceId);
    m_imageWidth = int(double(m_adb->devScreenSize().first) * m_imageScale);
    loop();
    m_adb->disconnectFromHost();
    m_adb->waitForDisconnected();
    m_adb->close();
    m_adb->deleteLater();
    m_adb = {};
}
