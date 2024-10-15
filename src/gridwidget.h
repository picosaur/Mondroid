#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H
#include <QGridLayout>
#include <QWidget>
#include "droidwidget.h"

class GridWidget : public QWidget
{
public:
    explicit GridWidget(QWidget *parent = nullptr);
    ~GridWidget();

    void init();

private:
    QGridLayout *m_gridLayout{};
    std::vector<DroidWidget *> m_droidWidgets;
};

#endif // SCROLLAREA_H
