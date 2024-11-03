#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H
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

    void init(const CellWidgetConf &conf);
    void free();
    void start();
    void stop();

    QList<QString> devList() const;

private slots:
    void onTimeout();

private:
    QTimer *m_timer{};
    CellWidgetConf m_cellConf{};
    QWidget *m_mainWidget{};
    QGridLayout *m_gridLayout{};
    std::vector<CellWidget *> m_cellWidgets{};
};

#endif // SCROLLAREA_H
