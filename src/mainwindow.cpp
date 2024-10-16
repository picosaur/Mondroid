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
    m_toolbar = new Toolbar();
    m_gridWidget = new GridWidget();

    setCentralWidget(m_gridWidget);
    addToolBar(m_toolbar);

    connect(m_toolbar, &Toolbar::start, m_gridWidget, &GridWidget::start);
    connect(m_toolbar, &Toolbar::stop, m_gridWidget, &GridWidget::stop);
}

MainWindow::~MainWindow()
{
    delete ui;
}
