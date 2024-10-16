#include "gridwidget.h"
#include <QDebug>
#include <QLabel>

GridWidget::GridWidget(QWidget *parent)
{
    m_gridLayout = new QGridLayout();

    for (int i{}; i != 2; ++i) {
        for (int j{}; j != 4; ++j) {
            auto droidw{new DroidWidget(this)};
            m_gridLayout->addWidget(droidw, i, j);
            m_droidWidgets.push_back(droidw);
        }
    }

    setLayout(m_gridLayout);
}

GridWidget::~GridWidget() {}

void GridWidget::start()
{
    for (auto dw : m_droidWidgets) {
        dw->start();
    }
}

void GridWidget::stop()
{
    for (auto dw : m_droidWidgets) {
        dw->stop();
    }
}
