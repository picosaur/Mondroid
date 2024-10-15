#include "gridwidget.h"
#include <QLabel>

GridWidget::GridWidget(QWidget *parent)
{
    m_gridLayout = new QGridLayout();

    for (int i{}; i != 2; ++i) {
        for (int j{}; j != 4; ++j) {
            auto droidw{new DroidWidget()};
            m_gridLayout->addWidget(droidw, i, j);
            m_droidWidgets.push_back(droidw);
        }
    }

    setLayout(m_gridLayout);
}

GridWidget::~GridWidget() {}

void GridWidget::init()
{
    m_droidWidgets.front()->init();
    for (auto dw : m_droidWidgets) {
    }
}
