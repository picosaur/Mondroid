#ifndef TOOLBAR_H
#define TOOLBAR_H
#include <QToolBar>

class QPushButton;

class Toolbar : public QToolBar
{
    Q_OBJECT

signals:
    void start();
    void stop();

public:
    Toolbar(QWidget *parent = nullptr);
    ~Toolbar();

private:
    QPushButton *m_startBtn{};
    QPushButton *m_stopBtn{};
};

#endif // TOOLBAR_H
