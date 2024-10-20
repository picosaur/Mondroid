#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H
#include <QWidget>

class DroidWidget;

class GridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GridWidget(QWidget *parent = nullptr);
    ~GridWidget();

    void init(int rows, int cols);
    void start();
    void stop();

private:
    QWidget *m_mainWidget{};
    std::vector<DroidWidget *> m_droidWidgets{};
};

#endif // SCROLLAREA_H
