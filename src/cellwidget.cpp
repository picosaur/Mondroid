#include "cellwidget.h"
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
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
    m_screen = new ScreenLabel();
    m_deviceInp = new QLineEdit();
    m_keInp = new QSpinBox();
    m_keBtn = new QPushButton("Send");

    connect(m_keBtn, &QPushButton::clicked, this, &CellWidget::onKeBtnClicked);
    connect(m_screen, &ScreenLabel::mouseMove, this, &CellWidget::onMouseMove);
    connect(m_screen, &ScreenLabel::mouseTap, this, &CellWidget::onMouseTap);
    connect(m_screen, &ScreenLabel::mouseSwipe, this, &CellWidget::onMouseSwipe);

    m_keInp->setMinimum(0);
    m_keInp->setMaximum(999);
    m_deviceInp->setReadOnly(true);
    m_deviceInp->setAlignment(Qt::AlignRight);
    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_area->setWidget(m_screen);

    m_toolLayout->addWidget(m_keInp);
    m_toolLayout->addWidget(m_keBtn);
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
    Q_ASSERT(m_adb == nullptr);
    Q_ASSERT(m_videoThread == nullptr);

    const auto s{m_deviceInp->text()};
    if (s.isEmpty()) {
        return;
    }

    m_adb = new AdbClient();
    m_adb->setHost(m_conf.host, m_conf.port);
    m_adb->setDevice(m_deviceInp->text());

    if (m_conf.fast) {
        auto videoThread = new FastVideoThread();
        m_videoThread = videoThread;
    } else {
        auto videoThread = new VideoThread();
        videoThread->setImageFormat(VideoThread::ImagePng);
        m_videoThread = videoThread;
    }

    m_videoThread->setHost(m_conf.host, m_conf.port);
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
    if (m_adb) {
        m_adb->disconnectFromHost();
        m_adb->waitForDisconnected();
        m_adb->close();
        m_adb->deleteLater();
        m_adb = {};
    }

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

void CellWidget::onMouseMove(const QPointF &p)
{
    emit mouseMove(toPoint(p));
}

void CellWidget::onMouseTap(const QPointF &p)
{
    auto p1 = toPoint(p);
    if (m_adb) {
        m_adb->inputTap(p1);
    }
    emit mouseTap(p1);
}

void CellWidget::onMouseSwipe(const QPointF &ps, const QPointF &pe, qint64 d)
{
    auto p1 = toPoint(ps);
    auto p2 = toPoint(pe);
    if (m_adb) {
        m_adb->inputSwipe(p1, p2, d);
    }
    emit mouseSwipe(p1, p2, d);
}

void CellWidget::onVideoFinished()
{
    m_videoThread = {};
}

void CellWidget::onKeBtnClicked()
{
    if (m_adb) {
        m_adb->inputKeyEvent(m_keInp->value());
    }
}

QPoint CellWidget::toPoint(const QPointF &p) const
{
    return QPoint(p.x() * 100.f / m_conf.scale, p.y() * 100.f / m_conf.scale);
}
