#include "gridwidget.h"
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QTimer>
#include "cellwidget.h"
#include "device/adbclient.h"

GridWidget::GridWidget(QWidget *parent)
{
    auto mainLayout = new QVBoxLayout();
    setLayout(mainLayout);
}

GridWidget::~GridWidget() {}

void GridWidget::initGrid(const GridConf &conf)
{
    /*
    if (conf.isEqualExceptScale(m_gridConf)) {
        m_gridConf = conf;
        for (auto it{m_cellWidgets.begin()}; it != m_cellWidgets.end(); ++it) {
            (*it)->setImageScale(m_gridConf.scale);
        }
        return;
    }*/

    auto currDevList = devList();

    freeGrid();
    m_gridConf = conf;
    auto gridLayout = new QGridLayout();
    for (int i{}, k{}; i != m_gridConf.rows; ++i) {
        for (int j{}; j != m_gridConf.cols; ++j, ++k) {
            auto cell = new CellWidget();
            cell->setGridConf(m_gridConf);
            cell->setCellConf(m_cellConf);
            if (!currDevList.empty()) {
                cell->setDevice(currDevList.takeFirst());
                cell->start();
            }
            connect(cell, &CellWidget::mouseMove, this, &GridWidget::mouseMove);
            connect(cell, &CellWidget::mouseTap, this, &GridWidget::mouseTap);
            connect(cell, &CellWidget::mouseSwipe, this, &GridWidget::mouseSwipe);
            gridLayout->addWidget(cell, i, j);
            m_cellWidgets.push_back(cell);
        }
    }
    auto mainWidget = new QWidget();
    mainWidget->setLayout(gridLayout);
    layout()->addWidget(mainWidget);

    setDiscover(m_gridConf.discover);
}

void GridWidget::freeGrid()
{
    if (layout() != nullptr) {
        QLayoutItem *item{};
        while ((item = layout()->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    }
    m_cellWidgets.clear();
}

void GridWidget::setCellConf(const CellConf &conf)
{
    m_cellConf = conf;
    for (auto it{m_cellWidgets.begin()}; it != m_cellWidgets.end(); ++it) {
        (*it)->setCellConf(m_cellConf);
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

void GridWidget::setDiscover(bool v)
{
    if (m_timer) {
        m_timer->stop();
    }
    if (v) {
        m_timer = new QTimer();
        connect(m_timer, &QTimer::timeout, this, &GridWidget::discoverDevices);
        m_timer->start(1000);
    }
}

void GridWidget::discoverDevices()
{
    const auto adbDevList{AdbClient::getDeviceList(m_gridConf.host, m_gridConf.port)};
    QList<QString> currDevList;
    for (auto it{m_cellWidgets.begin()}; it != m_cellWidgets.end(); ++it) {
        if (adbDevList.contains((*it)->device())) {
            currDevList.append((*it)->device());
        } else {
            (*it)->stop();
            (*it)->setDevice({});
        }
    }

    auto it{m_cellWidgets.begin()};
    for (auto jt{adbDevList.begin()}; jt != adbDevList.end(); ++jt) {
        if (currDevList.contains((*jt))) {
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
