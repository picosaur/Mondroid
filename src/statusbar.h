#ifndef STATUSBAR_H
#define STATUSBAR_H
#include <QStatusBar>

class QLineEdit;

class Statusbar : public QStatusBar
{
public:
    explicit Statusbar(QWidget *parent = nullptr);

public slots:
    void onMouseMove(const QPoint &p);
    void onMouseTap(const QPoint &p);
    void onMouseSwipe(const QPoint &p1, const QPoint &p2, qint64 d);

private:
    QLineEdit *m_moveOut{};
    QLineEdit *m_tapOut{};
    QLineEdit *m_swipeOut{};
};

#endif // STATUSBAR_H
