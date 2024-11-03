#include "cellwidget.h"
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include "device/adbclient.h"
#include "device/fastvideothread.h"
#include "device/videothread.h"

CellWidget::CellWidget(QWidget *parent)
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

    connect(m_aBtn, &QPushButton::clicked, this, &CellWidget::onABtnClicked);
    connect(m_bBtn, &QPushButton::clicked, this, &CellWidget::onBBtnClicked);
    connect(m_cBtn, &QPushButton::clicked, this, &CellWidget::onCBtnClicked);

    m_deviceInp->setReadOnly(true);
    m_deviceInp->setAlignment(Qt::AlignRight);
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

CellWidget::~CellWidget()
{
    stop();
}

void CellWidget::setDevice(const QString &deviceId)
{
    m_deviceInp->setText(deviceId);
}

void CellWidget::setConf(const CellWidgetConf &conf)
{
    m_conf = conf;
}

QString CellWidget::device() const
{
    return m_deviceInp->text();
}

const CellWidgetConf &CellWidget::conf() const
{
    return m_conf;
}

void CellWidget::start()
{
    const auto s{m_deviceInp->text()};
    if (s.isEmpty()) {
        return;
    }

    if (m_conf.fast) {
        auto videoThread = new FastVideoThread();
        m_videoThread = videoThread;
    } else {
        auto videoThread = new VideoThread();
        videoThread->setImageFormat(VideoThread::ImagePng);
        m_videoThread = videoThread;
    }

    m_videoThread->setDevice(m_deviceInp->text());
    m_videoThread->setImageScalePercent(m_conf.scale);
    m_videoThread->setImageRate(m_conf.rate);

    connect(m_videoThread, &VideoThread::imageReady, this, &CellWidget::updateScreen);
    connect(m_videoThread, &VideoThread::finished, this, &CellWidget::onVideoFinished);
    connect(m_videoThread, &VideoThread::finished, m_videoThread, &VideoThread::deleteLater);
    m_videoThread->start();
}

void CellWidget::stop()
{
    if (m_videoThread) {
        m_videoThread->requestInterruption();
    }
    m_screen->setPixmap({});
}

void CellWidget::updateScreen(const QImage &image)
{
    m_screen->setPixmap(QPixmap::fromImage(image));
    m_screen->setFixedSize(image.size());
}

void CellWidget::onVideoFinished()
{
    m_videoThread = {};
}

void CellWidget::onABtnClicked() {}

void CellWidget::onBBtnClicked() {}

void CellWidget::onCBtnClicked() {}
