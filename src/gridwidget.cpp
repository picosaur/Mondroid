#include "gridwidget.h"
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QTimer>
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
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GridWidget::onTimeout);
    m_timer->start(1000);
}

void GridWidget::stop()
{
    if (m_timer != nullptr) {
        m_timer->stop();
        m_timer->deleteLater();
    }
    for (auto dw : m_cellWidgets) {
        dw->stop();
    }
}

QList<QString> GridWidget::devList() const
{
    QList<QString> devList;
    for (auto it{m_cellWidgets.begin()}; it != m_cellWidgets.end(); ++it) {
        if (!(*it)->device().isEmpty()) {
            devList.append((*it)->device());
        }
    }
    return devList;
}

void GridWidget::onTimeout()
{
    const auto adbDevList{AdbClient::getDeviceList(m_cellConf.host, m_cellConf.port)};
    QList<QString> myDevList;
    for (auto it{m_cellWidgets.begin()}; it != m_cellWidgets.end(); ++it) {
        if (adbDevList.contains((*it)->device())) {
            myDevList.append((*it)->device());
        } else {
            (*it)->stop();
            (*it)->setDevice({});
        }
    }

    auto it{m_cellWidgets.begin()};
    for (auto jt{adbDevList.begin()}; jt != adbDevList.end(); ++jt) {
        if (myDevList.contains((*jt))) {
            continue;
        }
        for (; it != m_cellWidgets.end(); ++it) {
            if ((*it)->device().isEmpty()) {
                (*it)->setDevice((*jt));
                (*it)->start();
                break;
            }
        }
        if (it == m_cellWidgets.end()) {
            break;
        }
    }
}
