#include "statusbar.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

Statusbar::Statusbar(QWidget *parent)
    : QStatusBar{parent}
{
    m_moveOut = new QLineEdit();
    addWidget(new QLabel("Move"));
    addWidget(m_moveOut);

    m_tapOut = new QLineEdit();
    addWidget(new QLabel("Tap"));
    addWidget(m_tapOut);

    m_swipeOut = new QLineEdit();
    addWidget(new QLabel("Swipe"));
    addWidget(m_swipeOut);
}

void Statusbar::onMouseMove(const QPoint &p)
{
    m_moveOut->setText(QString::number(p.x()) + " " + QString::number(p.y()));
}

void Statusbar::onMouseTap(const QPoint &p)
{
    m_tapOut->setText(QString::number(p.x()) + " " + QString::number(p.y()));
}

void Statusbar::onMouseSwipe(const QPoint &p1, const QPoint &p2, qint64 d)
{
    m_swipeOut->setText(QString::number(p1.x()) + " " + QString::number(p1.y()) + " "
                        + QString::number(p2.x()) + " " + QString::number(p2.y()) + " "
                        + QString::number(d));
}
