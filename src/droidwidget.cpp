#include "droidwidget.h"
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include "device/adbclient.h"
#include "device/videothread.h"

DroidWidget::DroidWidget(QWidget *parent)
    : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout();
    m_toolLayout = new QHBoxLayout();
    m_area = new QScrollArea();
    m_screen = new QLabel();
    m_deviceInp = new QLineEdit();
    m_aBtn = new QPushButton("A");
    m_bBtn = new QPushButton("B");
    m_cBtn = new QPushButton("C");

    const QSize btnSize{25, 25};
    m_aBtn->setFixedSize(btnSize);
    m_bBtn->setFixedSize(btnSize);
    m_cBtn->setFixedSize(btnSize);

    connect(m_aBtn, &QPushButton::clicked, this, &DroidWidget::onABtnClicked);
    connect(m_bBtn, &QPushButton::clicked, this, &DroidWidget::onBBtnClicked);
    connect(m_cBtn, &QPushButton::clicked, this, &DroidWidget::onCBtnClicked);

    //m_deviceInp->setReadOnly(true);
    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_area->setWidget(m_screen);

    m_toolLayout->addWidget(m_aBtn);
    m_toolLayout->addWidget(m_bBtn);
    m_toolLayout->addWidget(m_cBtn);
    m_toolLayout->addWidget(m_deviceInp);

    m_mainLayout->addLayout(m_toolLayout);
    m_mainLayout->addWidget(m_area);

    setLayout(m_mainLayout);
}

DroidWidget::~DroidWidget()
{
    stop();
}

void DroidWidget::setDeviceId(const QString &deviceId)
{
    m_deviceInp->setText(deviceId.split(":").first());
}

void DroidWidget::start()
{
    const auto s{m_deviceInp->text()};
    if (s.isEmpty()) {
        return;
    }
    m_videoThread = new VideoThread();
    m_videoThread->setDeviceId(m_deviceInp->text());
    m_videoThread->setVideoMode(VideoThread::NativeRaw);
    connect(m_videoThread, &VideoThread::imageReady, this, &DroidWidget::updateScreen);
    connect(m_videoThread, &VideoThread::finished, this, &DroidWidget::onVideoFinished);
    connect(m_videoThread, &VideoThread::finished, m_videoThread, &VideoThread::deleteLater);
    m_videoThread->start();
}

void DroidWidget::stop()
{
    if (m_videoThread) {
        m_videoThread->requestInterruption();
    }
}

void DroidWidget::updateScreen(const QImage &image)
{
    m_screen->setPixmap(QPixmap::fromImage(image));
    m_screen->setFixedSize(image.size());
}

void DroidWidget::onVideoFinished()
{
    m_videoThread = {};
}

void DroidWidget::onABtnClicked() {}

void DroidWidget::onBBtnClicked() {}

void DroidWidget::onCBtnClicked() {}
