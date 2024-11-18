#include "statusbar.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

Statusbar::Statusbar(QWidget *parent)
    : QStatusBar{parent}
{
    m_moveOut = new QLineEdit();
    m_moveOut->setFixedWidth(100.f);
    addWidget(new QLabel("Pos"));
    addWidget(m_moveOut);

    m_tapOut = new QLineEdit();
    m_tapOut->setFixedWidth(100.f);
    addWidget(new QLabel("Tap"));
    addWidget(m_tapOut);

    m_swipeOut = new QLineEdit();
    m_swipeOut->setFixedWidth(200.f);
    addWidget(new QLabel("Swipe"));
    addWidget(m_swipeOut);

    m_devFlag = new QCheckBox("Dev");
    m_kevFlag = new QCheckBox("KEv");
    m_cmdFlag = new QCheckBox("Cmd");
    m_resFlag = new QCheckBox("Out");

    m_resSizeInp = new QSpinBox();
    m_resSizeInp->setMinimum(0);
    m_resSizeInp->setMaximum(99);
    m_resSizeInp->setPrefix("L:");

    addWidget(m_devFlag);
    addWidget(m_kevFlag);
    addWidget(m_cmdFlag);
    addWidget(m_resFlag);
    addWidget(m_resSizeInp);

    connect(m_devFlag, &QCheckBox::stateChanged, this, &Statusbar::confChanged);
    connect(m_kevFlag, &QCheckBox::stateChanged, this, &Statusbar::confChanged);
    connect(m_cmdFlag, &QCheckBox::stateChanged, this, &Statusbar::confChanged);
    connect(m_resFlag, &QCheckBox::stateChanged, this, &Statusbar::confChanged);
    connect(m_resSizeInp, SIGNAL(valueChanged(int)), this, SIGNAL(confChanged()));
}

void Statusbar::saveState(QSettings &settings) const
{
    settings.setValue("statusbar/devFlag", m_devFlag->checkState());
    settings.setValue("statusbar/kevFlag", m_kevFlag->checkState());
    settings.setValue("statusbar/cmdFlag", m_cmdFlag->checkState());
    settings.setValue("statusbar/resFlag", m_resFlag->checkState());
    settings.setValue("statusbar/resSizeInp", m_resSizeInp->value());
}

void Statusbar::loadState(const QSettings &settings)
{
    m_devFlag->setCheckState(settings.value("statusbar/devFlag").value<Qt::CheckState>());
    m_kevFlag->setCheckState(settings.value("statusbar/kevFlag").value<Qt::CheckState>());
    m_cmdFlag->setCheckState(settings.value("statusbar/cmdFlag").value<Qt::CheckState>());
    m_resFlag->setCheckState(settings.value("statusbar/resFlag").value<Qt::CheckState>());
    m_resSizeInp->setValue(settings.value("statusbar/resSizeInp").value<int>());
}

bool Statusbar::showDev() const
{
    return m_devFlag->isChecked();
}

bool Statusbar::showKev() const
{
    return m_kevFlag->isChecked();
}

bool Statusbar::showCmd() const
{
    return m_cmdFlag->isChecked();
}

bool Statusbar::showRes() const
{
    return m_resFlag->isChecked();
}

int Statusbar::resSize() const
{
    return m_resSizeInp->value();
}

CellConf Statusbar::cellConf() const
{
    CellConf conf;
    conf.devVisible = showDev();
    conf.kevVisible = showKev();
    conf.cmdVisible = showCmd();
    conf.resVisible = showRes();
    conf.resOutSize = resSize();
    return conf;
}

void Statusbar::onMouseMove(const QPoint &p)
{
    m_moveOut->setText(QString::number(p.x()) + " " + QString::number(p.y()));
}

void Statusbar::onMouseTap(const QPoint &p)
{
    m_tapOut->setText(QString::number(p.x()) + " " + QString::number(p.y()));
}

void Statusbar::onMouseSwipe(const QPoint &p1, const QPoint &p2, qint64 d)
{
    m_swipeOut->setText(QString::number(p1.x()) + " " + QString::number(p1.y()) + " "
                        + QString::number(p2.x()) + " " + QString::number(p2.y()) + " "
                        + QString::number(d));
}
