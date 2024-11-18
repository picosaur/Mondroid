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

    QSettings settings("settings.ini", QSettings::IniFormat);
    m_toolbar->loadState(settings);
    m_statusbar->loadState(settings);

    m_gridWidget->initGrid(m_toolbar->gridConf());
    m_gridWidget->setCellConf(m_statusbar->cellConf());

    connect(m_toolbar, &Toolbar::confChanged, this, &MainWindow::onGridConfChanged);
    connect(m_statusbar, &Statusbar::confChanged, this, &MainWindow::onCellConfChanged);

    connect(m_gridWidget, &GridWidget::mouseMove, m_statusbar, &Statusbar::onMouseMove);
    connect(m_gridWidget, &GridWidget::mouseTap, m_statusbar, &Statusbar::onMouseTap);
    connect(m_gridWidget, &GridWidget::mouseSwipe, m_statusbar, &Statusbar::onMouseSwipe);
}

MainWindow::~MainWindow()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    m_toolbar->saveState(settings);
    m_statusbar->saveState(settings);
    delete ui;
}

void MainWindow::onGridConfChanged()
{
    m_gridWidget->initGrid(m_toolbar->gridConf());
}

void MainWindow::onCellConfChanged()
{
    m_gridWidget->setCellConf(m_statusbar->cellConf());
}
