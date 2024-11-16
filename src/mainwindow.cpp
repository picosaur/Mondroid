#include "mainwindow.h"
#include <QDebug>
#include <QLibraryInfo>
#include <QMouseEvent>
#include <QSettings>
#include <QTimer>
#include "gridwidget.h"
#include "statusbar.h"
#include "toolbar.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
	ui->setupUi(this);
    m_toolbar = new Toolbar();
    m_statusbar = new Statusbar();
    m_gridWidget = new GridWidget();

    setCentralWidget(m_gridWidget);
    addToolBar(m_toolbar);
    setStatusBar(m_statusbar);

    connect(m_toolbar, &Toolbar::runStateChanged, this, &MainWindow::onRunStateChanged);
    connect(m_toolbar, &Toolbar::applyConfRequested, this, &MainWindow::onApplyConfRequested);

    connect(m_statusbar, &Statusbar::showDevChanged, m_gridWidget, &GridWidget::setDevVisible);
    connect(m_statusbar, &Statusbar::showKevChanged, m_gridWidget, &GridWidget::setKevVisible);
    connect(m_statusbar, &Statusbar::showCmdChanged, m_gridWidget, &GridWidget::setCmdVisible);
    connect(m_statusbar, &Statusbar::showResChanged, m_gridWidget, &GridWidget::setResVisible);
    connect(m_statusbar, &Statusbar::resSizeChanged, m_gridWidget, &GridWidget::setResOutSize);

    connect(m_gridWidget, &GridWidget::mouseMove, m_statusbar, &Statusbar::onMouseMove);
    connect(m_gridWidget, &GridWidget::mouseTap, m_statusbar, &Statusbar::onMouseTap);
    connect(m_gridWidget, &GridWidget::mouseSwipe, m_statusbar, &Statusbar::onMouseSwipe);

    QSettings settings("settings.ini", QSettings::IniFormat);
    m_toolbar->loadState(settings);
    m_statusbar->loadState(settings);

    m_gridWidget->setDevVisible(m_statusbar->showDev());
    m_gridWidget->setKevVisible(m_statusbar->showKev());
    m_gridWidget->setCmdVisible(m_statusbar->showCmd());
    m_gridWidget->setResVisible(m_statusbar->showRes());
    m_gridWidget->setResOutSize(m_statusbar->resSize());
}

MainWindow::~MainWindow()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    m_toolbar->saveState(settings);
    m_statusbar->saveState(settings);
    delete ui;
}

void MainWindow::onRunStateChanged(int state)
{
    startStop();
}

void MainWindow::onApplyConfRequested()
{
    startStop();
}

void MainWindow::startStop()
{
    m_gridWidget->stop();
    m_gridWidget->init(m_toolbar->cellConf());
    if (m_toolbar->runState() > 0) {
        m_gridWidget->start();
    }
}
