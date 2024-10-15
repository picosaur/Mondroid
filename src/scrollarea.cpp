#include "scrollarea.h"
#include <QLabel>

ScrollArea::ScrollArea(QWidget *parent)
    : QScrollArea(parent)
{
    m_gridLayout = new QGridLayout();
    m_gridWidget = new QWidget();
    m_gridWidget->setLayout(m_gridLayout);
    m_gridWidget->setFixedSize(1920, 1080);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setWidget(m_gridWidget);

    for (int i{}; i != 3; ++i) {
        for (int j{}; j != 4; ++j) {
            auto droidw{new DroidWidget()};
            m_gridLayout->addWidget(droidw, i, j);
            m_droidWidgets.push_back(droidw);
        }
    }
}

ScrollArea::~ScrollArea() {}

void ScrollArea::init()
{
    m_droidWidgets.front()->init();
    for (auto dw : m_droidWidgets) {
    }
}
