#include "toolbar.h"
#include <QPushButton>

Toolbar::Toolbar(QWidget *parent)
    : QToolBar(parent)
{
    m_startBtn = new QPushButton("Start");
    m_stopBtn = new QPushButton("Stop");

    connect(m_startBtn, &QPushButton::clicked, this, &Toolbar::start);
    connect(m_stopBtn, &QPushButton::clicked, this, &Toolbar::stop);

    addWidget(m_startBtn);
    addWidget(m_stopBtn);
}

Toolbar::~Toolbar() {}
