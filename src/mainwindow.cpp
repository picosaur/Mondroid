/*
   DivvyDroid - Application to screencast and remote control Android devices.

   Copyright (C) 2019 - Mladen Milinkovic <maxrd2@smoothware.net>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "mainwindow.h"
#include <QDebug>
#include <QLibraryInfo>
#include <QMouseEvent>
#include <QTimer>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
	ui->setupUi(this);
    m_scrollArea = new ScrollArea();
    m_toolbar = new Toolbar();

    setCentralWidget(m_scrollArea);
    addToolBar(m_toolbar);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    m_scrollArea->init();
}
