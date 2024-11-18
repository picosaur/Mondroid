#include "cellwidget.h"
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPlainTextEdit>
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

    m_area = new QScrollArea();
    m_screen = new ScreenLabel();

    m_devLayout = new QHBoxLayout();
    m_devLabel = new QLabel("Device");
    m_devInp = new QLineEdit();

    m_kevLayout = new QHBoxLayout();
    m_kevLabel = new QLabel("Key Event");
    m_kevInp = new QSpinBox();
    m_kevBtn = new QPushButton("Send");

    m_cmdLayout = new QHBoxLayout();
    m_cmdLabel = new QLabel("Command");
    m_cmdBtn = new QPushButton("Send");
    m_cmdInp = new QLineEdit();

    m_resLayout = new QHBoxLayout();
    m_resLabel = new QLabel("Output");
    m_resOut = new QPlainTextEdit();

    connect(m_kevBtn, &QPushButton::clicked, this, &CellWidget::onKevBtnClicked);
    connect(m_cmdBtn, &QPushButton::clicked, this, &CellWidget::onCmdBtnClicked);

    connect(m_screen, &ScreenLabel::mouseMove, this, &CellWidget::onMouseMove);
    connect(m_screen, &ScreenLabel::mouseTap, this, &CellWidget::onMouseTap);
    connect(m_screen, &ScreenLabel::mouseSwipe, this, &CellWidget::onMouseSwipe);

    QFont monoFont("Monospace");
    monoFont.setStyleHint(QFont::TypeWriter);

    //m_deviceInp->setAlignment(Qt::AlignRight);
    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_area->setWidget(m_screen);

    m_devInp->setReadOnly(true);
    m_devLabel->setFixedWidth(80.f);
    m_devLayout->addWidget(m_devLabel);
    m_devLayout->addWidget(m_devInp);

    m_kevInp->setMinimum(0);
    m_kevInp->setMaximum(999);
    m_kevInp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_kevLabel->setFixedWidth(80.f);
    m_kevLayout->addWidget(m_kevLabel);
    m_kevLayout->addWidget(m_kevInp);
    m_kevLayout->addWidget(m_kevBtn);

    m_cmdLabel->setFixedWidth(80.f);
    m_cmdInp->setFont(monoFont);
    m_cmdLayout->addWidget(m_cmdLabel);
    m_cmdLayout->addWidget(m_cmdInp);
    m_cmdLayout->addWidget(m_cmdBtn);

    m_resLabel->setFixedWidth(80.f);
    m_resOut->setFont(monoFont);
    m_resOut->setReadOnly(true);
    setResOutSize(2);
    m_resLayout->addWidget(m_resLabel);
    m_resLayout->addWidget(m_resOut);

    m_mainLayout->addLayout(m_devLayout);
    m_mainLayout->addLayout(m_kevLayout);
    m_mainLayout->addLayout(m_cmdLayout);
    m_mainLayout->addLayout(m_resLayout);
    m_mainLayout->addWidget(m_area);

    setLayout(m_mainLayout);
}

CellWidget::~CellWidget()
{
    stop();
}

void CellWidget::setDevice(const QString &deviceId)
{
    m_devInp->setText(deviceId);
}

QString CellWidget::device() const
{
    return m_devInp->text();
}

void CellWidget::setCellConf(const CellConf &conf)
{
    m_cellConf = conf;
    setDevVisible(m_cellConf.devVisible);
    setKevVisible(m_cellConf.kevVisible);
    setCmdVisible(m_cellConf.cmdVisible);
    setResVisible(m_cellConf.resVisible);
    setResOutSize(m_cellConf.resOutSize);
}

const CellConf &CellWidget::cellConf() const
{
    return m_cellConf;
}

void CellWidget::setGridConf(const GridConf &conf)
{
    m_gridConf = conf;
}

const GridConf &CellWidget::gridConf() const
{
    return m_gridConf;
}

void CellWidget::setImageScale(int scale)
{
    m_gridConf.scale = scale;
    if (m_videoThread) {
        m_videoThread->setImageScalePercent(scale);
    }
}

void CellWidget::start()
{
    stop();

    const auto s{m_devInp->text()};
    if (s.isEmpty()) {
        return;
    }

    m_adb = new AdbClient();
    m_adb->setHost(m_gridConf.host, m_gridConf.port);
    m_adb->setDevice(m_devInp->text());

    if (m_gridConf.fast) {
        auto videoThread = new FastVideoThread();
        m_videoThread = videoThread;
    } else {
        auto videoThread = new VideoThread();
        videoThread->setImageFormat(VideoThread::ImagePng);
        m_videoThread = videoThread;
    }

    m_videoThread->setHost(m_gridConf.host, m_gridConf.port);
    m_videoThread->setDevice(m_devInp->text());
    m_videoThread->setImageScalePercent(m_gridConf.scale);
    m_videoThread->setImageRate(m_gridConf.rate);

    connect(m_videoThread, &VideoThread::imageReady, this, &CellWidget::onUpdateScreen);
    connect(m_videoThread, &VideoThread::finished, this, &CellWidget::onVideoFinished);
    m_videoThread->start();
}

void CellWidget::stop()
{
    if (m_adb) {
        delete m_adb;
    }
    if (m_videoThread) {
        delete m_videoThread;
    }
    m_screen->setPixmap({});
}

void CellWidget::setDevVisible(bool v)
{
    m_devInp->setVisible(v);
    m_devLabel->setVisible(v);
}

void CellWidget::setKevVisible(bool v)
{
    m_kevBtn->setVisible(v);
    m_kevInp->setVisible(v);
    m_kevLabel->setVisible(v);
}

void CellWidget::setCmdVisible(bool v)
{
    m_cmdLabel->setVisible(v);
    m_cmdInp->setVisible(v);
    m_cmdBtn->setVisible(v);
}

void CellWidget::setResVisible(bool v)
{
    m_resLabel->setVisible(v);
    m_resOut->setVisible(v);
}

void CellWidget::setResOutSize(int sz)
{
    m_resOut->setFixedHeight(m_resOut->fontMetrics().lineSpacing() * (sz + 1));
}

void CellWidget::onUpdateScreen(const QImage &image)
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
    m_videoThread->deleteLater();
    m_videoThread = {};
}

void CellWidget::onKevBtnClicked()
{
    if (m_adb) {
        m_resOut->setPlainText(m_adb->inputKeyEvent(m_kevInp->value()));
    }
}

void CellWidget::onCmdBtnClicked()
{
    if (m_adb) {
        m_resOut->setPlainText(m_adb->sendToDevice(m_cmdInp->text()));
    }
}

QPoint CellWidget::toPoint(const QPointF &p) const
{
    return QPoint(p.x() * 100.f / m_gridConf.scale, p.y() * 100.f / m_gridConf.scale);
}
