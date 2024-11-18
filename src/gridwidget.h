#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H
#include <QPointer>
#include <QWidget>
#include "cellwidget.h"

class QGridLayout;
class QTimer;

class GridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GridWidget(QWidget *parent = nullptr);
    ~GridWidget();

    void initGrid(const GridConf &conf);
    void freeGrid();

    void setCellConf(const CellConf &conf);

    QList<QString> devList() const;

signals:
    void mouseMove(QPoint);
    void mouseTap(QPoint);
    void mouseSwipe(QPoint, QPoint, qint64);

public slots:
    void setDiscover(bool v);
    void discoverDevices();

private:
    GridConf m_gridConf{};
    CellConf m_cellConf{};
    QPointer<QTimer> m_timer{};
    std::vector<CellWidget *> m_cellWidgets{};
};

#endif // SCROLLAREA_H
