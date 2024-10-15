#include "droidwidget.h"
#include <QLineEdit>
#include <qboxlayout.h>

DroidWidget::DroidWidget(QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QVBoxLayout();
    m_area = new QScrollArea();
    m_screen = new QLabel();

    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_area->setWidget(m_screen);

    m_layout->addWidget(m_area);
    m_layout->addWidget(new QLineEdit("192.168.1.122"));

    setLayout(m_layout);
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
    m_videoThread = new VideoThread();
    m_videoThread->setVideoMode(VideoThread::FastH264);
    m_videoThread->setImageSize(720, 1280);
    connect(m_videoThread, &VideoThread::imageReady, this, &DroidWidget::updateScreen);
    m_videoThread->start();
}

void DroidWidget::onInputReady() {}

void DroidWidget::updateScreen(const QImage &image)
{
    m_screen->setPixmap(QPixmap::fromImage(image));
    m_screen->setFixedSize(image.size());
}
