#include "droidwidget.h"
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QVBoxLayout>
#include "device/videothread.h"
#include "initthread.h"
#include <qboxlayout.h>

DroidWidget::DroidWidget(QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QVBoxLayout();
    m_area = new QScrollArea();
    m_screen = new QLabel();
    m_hostInp = new QLineEdit();

    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_area->setWidget(m_screen);

    m_layout->addWidget(m_hostInp);
    m_layout->addWidget(m_area);

    setLayout(m_layout);
}

DroidWidget::~DroidWidget()
{
    stop();
}

void DroidWidget::start()
{
    stop();
    const auto s{m_hostInp->text()};
    if (s.isEmpty()) {
        return;
    }
    m_initThread = new InitThread(s);
    connect(m_initThread, &InitThread::deviceConnected, this, &DroidWidget::onDeviceReady);
    connect(m_initThread, &InitThread::inputReady, this, &DroidWidget::onInputReady);
    m_initThread->start();
}

void DroidWidget::stop()
{
    if (m_initThread) {
        m_initThread->disconnect(this);
        m_initThread->requestInterruption();
        m_initThread->deleteLater();
    }
    if (m_videoThread) {
        m_videoThread->disconnect(this);
        m_videoThread->requestInterruption();
        m_videoThread->deleteLater();
    }
}

void DroidWidget::onDeviceReady()
{
    m_videoThread = new VideoThread();
    m_videoThread->setVideoMode(VideoThread::FastH264);
    connect(m_videoThread, &VideoThread::imageReady, this, &DroidWidget::updateScreen);
    m_videoThread->start();
}

void DroidWidget::onInputReady() {}

void DroidWidget::updateScreen(const QImage &image)
{
    m_screen->setPixmap(QPixmap::fromImage(image));
    m_screen->setFixedSize(image.size());
}
