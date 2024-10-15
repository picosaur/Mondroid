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

    for (int i{}; i != 6; ++i) {
        for (int j{}; j != 6; ++j) {
            auto droidw{new DroidWidget()};
            droidw->setFixedSize({426, 240});
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
