#include "droidwidget.h"
#include <qboxlayout.h>

DroidWidget::DroidWidget(QWidget *parent)
    : QWidget(parent)
{
    m_screen = new QLabel();
    m_layout = new QVBoxLayout();
    setLayout(m_layout);
    m_layout->addWidget(m_screen);

    QPixmap img(854, 480);
    img.fill(Qt::black);
    m_screen->setPixmap(img);
}

DroidWidget::~DroidWidget()
{
    delete m_initThread;
    delete m_videoThread;
}

void DroidWidget::init()
{
    m_initThread = new InitThread();
    connect(m_initThread, &InitThread::deviceConnected, this, &DroidWidget::onDeviceReady);
    connect(m_initThread, &InitThread::inputReady, this, &DroidWidget::onInputReady);
    m_initThread->start();
}

void DroidWidget::onDeviceReady()
{
    // start video thread
    m_videoThread = new VideoThread();
    connect(m_videoThread, &VideoThread::imageReady, this, &DroidWidget::updateScreen);
    m_videoThread->start();
}

void DroidWidget::onInputReady() {}

void DroidWidget::updateScreen(const QImage &image)
{
    m_screen->setPixmap(QPixmap::fromImage(image));
}
