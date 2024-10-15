#ifndef SCROLLAREA_H
#define SCROLLAREA_H
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include "droidwidget.h"

class ScrollArea : public QScrollArea
{
public:
    explicit ScrollArea(QWidget *parent = nullptr);
    ~ScrollArea();

    void init();

private:
    QWidget *m_gridWidget{};
    QGridLayout *m_gridLayout{};
    std::vector<DroidWidget *> m_droidWidgets;
};

#endif // SCROLLAREA_H
