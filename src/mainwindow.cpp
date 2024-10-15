#include "mainwindow.h"
#include <QDebug>
#include <QLibraryInfo>
#include <QMouseEvent>
#include <QTimer>
#include "gridwidget.h"
#include "toolbar.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
	ui->setupUi(this);
    m_gridWidget = new GridWidget();
    m_toolbar = new Toolbar();

    setCentralWidget(m_gridWidget);
    addToolBar(m_toolbar);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    m_gridWidget->init();
}
