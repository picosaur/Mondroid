#include "mainwindow.h"
#include <QDebug>
#include <QLibraryInfo>
#include <QMouseEvent>
#include <QSettings>
#include <QTimer>
#include "gridwidget.h"
#include "toolbar.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
	ui->setupUi(this);
    m_toolbar = new Toolbar();
    m_gridWidget = new GridWidget();

    setCentralWidget(m_gridWidget);
    addToolBar(m_toolbar);

    connect(m_toolbar, &Toolbar::runStateChanged, this, &MainWindow::onRunStateChanged);
    connect(m_toolbar, &Toolbar::applyConfRequested, this, &MainWindow::onApplyConfRequested);

    QSettings settings("settings.ini", QSettings::IniFormat);
    m_toolbar->loadState(settings);
}

MainWindow::~MainWindow()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    m_toolbar->saveState(settings);
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
