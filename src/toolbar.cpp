#include "toolbar.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QSpinBox>

Toolbar::Toolbar(QWidget *parent)
    : QToolBar(parent)
{
    m_startBtn = new QPushButton("Start");
    m_stopBtn = new QPushButton("Stop");
    m_rowsInp = new QSpinBox();
    m_colsInp = new QSpinBox();
    m_hostInp = new QLineEdit();
    m_portInp = new QSpinBox();
    m_scaleInp = new QSpinBox();

    m_rowsInp->setFixedSize(60, 30);
    m_rowsInp->setMinimum(1);
    m_rowsInp->setMaximum(9);
    m_rowsInp->setValue(2);

    m_colsInp->setFixedSize(60, 30);
    m_colsInp->setMinimum(1);
    m_colsInp->setMaximum(9);
    m_colsInp->setValue(4);

    m_hostInp->setText("127.0.0.1");
    m_hostInp->setFixedSize(150, 30);

    m_portInp->setMinimum(1);
    m_portInp->setMaximum(65535);
    m_portInp->setValue(5037);
    m_portInp->setFixedSize(60, 30);

    m_scaleInp->setMinimum(1);
    m_scaleInp->setMaximum(10);
    m_scaleInp->setFixedSize(60, 30);

    addWidget(m_startBtn);
    addWidget(m_stopBtn);
    addSeparator();
    addWidget(new QLabel("Rows"));
    addWidget(m_rowsInp);
    addWidget(new QLabel("Cols"));
    addWidget(m_colsInp);
    addSeparator();
    addWidget(new QLabel("Scale"));
    addWidget(m_scaleInp);
    addSeparator();
    addWidget(new QLabel("Host"));
    addWidget(m_hostInp);
    addWidget(new QLabel("Port"));
    addWidget(m_portInp);

    connect(m_startBtn, &QPushButton::clicked, this, &Toolbar::start);
    connect(m_stopBtn, &QPushButton::clicked, this, &Toolbar::stop);
}

Toolbar::~Toolbar() {}

int Toolbar::rows() const
{
    return m_rowsInp->value();
}

int Toolbar::cols() const
{
    return m_colsInp->value();
}

int Toolbar::scale() const
{
    return m_scaleInp->value();
}

QString Toolbar::host() const
{
    return m_hostInp->text();
}

int Toolbar::port() const
{
    return m_portInp->value();
}
