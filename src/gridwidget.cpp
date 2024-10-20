#include "gridwidget.h"
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include "device/adbclient.h"
#include "droidwidget.h"

GridWidget::GridWidget(QWidget *parent)
{
    setLayout(new QVBoxLayout());
}

GridWidget::~GridWidget() {}

void GridWidget::init(int rows, int cols)
{
    auto grid = new QGridLayout();
    m_droidWidgets.clear();
    for (int i{}; i != rows; ++i) {
        for (int j{}; j != cols; ++j) {
            auto droidw{new DroidWidget(this)};
            grid->addWidget(droidw, i, j);
            m_droidWidgets.push_back(droidw);
        }
    }
    if (m_mainWidget != nullptr) {
        layout()->removeWidget(m_mainWidget);
        m_mainWidget->deleteLater();
    }
    m_mainWidget = new QWidget();
    m_mainWidget->setLayout(grid);
    layout()->addWidget(m_mainWidget);
}

void GridWidget::start()
{
    const auto adb{new AdbClient()};
    auto devList{adb->getDeviceList()};
    adb->deleteLater();

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
}

void GridWidget::stop()
{
    for (auto dw : m_droidWidgets) {
        dw->stop();
    }
}
