#ifndef TOOLBAR_H
#define TOOLBAR_H
#include <QToolBar>

class Toolbar : public QToolBar
{
    Q_OBJECT

public:
    Toolbar(QWidget *parent = nullptr);
    ~Toolbar();
};

#endif // TOOLBAR_H
