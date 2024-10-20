#include "gridwidget.h"
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include "cellwidget.h"
#include "device/adbclient.h"

GridWidget::GridWidget(QWidget *parent)
{
    setLayout(new QVBoxLayout());
}

GridWidget::~GridWidget() {}

void GridWidget::init(const CellWidgetConf &conf)
{
    m_cellConf = conf;

    free();

    m_gridLayout = new QGridLayout();
    for (int i{}; i != m_cellConf.rows; ++i) {
        for (int j{}; j != m_cellConf.cols; ++j) {
            auto cell{new CellWidget(this)};
            cell->setConf(m_cellConf);
            m_gridLayout->addWidget(cell, i, j);
            m_cellWidgets.push_back(cell);
        }
    }
    m_mainWidget = new QWidget();
    m_mainWidget->setLayout(m_gridLayout);
    layout()->addWidget(m_mainWidget);
}

void GridWidget::free()
{
    if (m_mainWidget) {
        layout()->removeWidget(m_mainWidget);
        m_mainWidget->deleteLater();
        m_gridLayout->deleteLater();
        for (auto &w : m_cellWidgets) {
            w->deleteLater();
        }
        m_cellWidgets.clear();
    }
}

void GridWidget::start()
{
    const auto devList{AdbClient::getDeviceList(m_cellConf.host, m_cellConf.port)};
    auto it{m_cellWidgets.begin()};
    for (auto &devId : devList) {
        (*it)->setDevice(devId);
        it++;
        if (it == m_cellWidgets.end()) {
            break;
        }
    }
    for (auto dw : m_cellWidgets) {
        dw->start();
    }
}

void GridWidget::stop()
{
    for (auto dw : m_cellWidgets) {
        dw->stop();
    }
}
