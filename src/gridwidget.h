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

signals:
    void mouseMove(QPoint);
    void mouseTap(QPoint);
    void mouseSwipe(QPoint, QPoint, qint64);

public slots:
    void setDevVisible(bool v);
    void setKevVisible(bool v);
    void setCmdVisible(bool v);

private slots:
    void onTimeout();

private:
    QTimer *m_timer{};
    CellWidgetConf m_cellConf{};
    QWidget *m_mainWidget{};
    QGridLayout *m_gridLayout{};
    std::vector<CellWidget *> m_cellWidgets{};
    bool m_devVisible{}, m_kevVisible{}, m_cmdVisible{};
};

#endif // SCROLLAREA_H
