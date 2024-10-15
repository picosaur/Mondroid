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
#include "ui_mainwindow.h"
#include "device/deviceinfo.h"
#include <QMouseEvent>
#include <QDebug>
#include <QLibraryInfo>

#define STR_(x) #x
#define STR(x) STR_(x)

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	  ui(new Ui::MainWindow),
	  m_initThread(nullptr),
	  m_videoThread(nullptr)
{
	ui->setupUi(this);
	ui->screen->setFocusPolicy(Qt::StrongFocus);
	ui->screen->setFocus();
}

MainWindow::~MainWindow()
{
	delete m_initThread;
	delete m_videoThread;
	delete ui;
}

void MainWindow::init()
{
	m_initThread = new InitThread();
	connect(m_initThread, &InitThread::deviceConnected, this, &MainWindow::onDeviceReady);
	connect(m_initThread, &InitThread::inputReady, this, &MainWindow::onInputReady);
	m_initThread->start();
}

void MainWindow::onDeviceReady()
{
	QPixmap img(IMAGE_WIDTH, IMAGE_WIDTH * aDev->screenHeight() / aDev->screenWidth());
	img.fill(Qt::black);
	ui->screen->setPixmap(img);
    //adjustSize();
    //setFixedSize(sizeHint());

    // start video thread
	m_videoThread = new VideoThread();
	connect(m_videoThread, &VideoThread::imageReady, this, &MainWindow::updateScreen);
	m_videoThread->start();
}

void MainWindow::onInputReady() {}

void MainWindow::updateScreen(const QImage &image)
{
	ui->screen->setPixmap(QPixmap::fromImage(image));
}
