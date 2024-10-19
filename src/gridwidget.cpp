#include "gridwidget.h"
#include <QDebug>
#include <QLabel>
#include "device/adbclient.h"

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
    const auto adb{new AdbClient()};
    auto devList{adb->getDeviceList()};
    auto dwIt{m_droidWidgets.begin()};
    for (auto &item : devList) {
        (*dwIt)->setDeviceId(item);
        dwIt++;
        if (dwIt == m_droidWidgets.end()) {
            break;
        }
    }
    for (auto dw : m_droidWidgets) {
        dw->start();
    }
    adb->deleteLater();
}

void GridWidget::stop()
{
    for (auto dw : m_droidWidgets) {
        dw->stop();
    }
}
