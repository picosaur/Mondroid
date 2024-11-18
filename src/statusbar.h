#ifndef STATUSBAR_H
#define STATUSBAR_H
#include <QSettings>
#include <QStatusBar>
#include "cellwidget.h"

class QLineEdit;
class QCheckBox;
class QSpinBox;

class Statusbar : public QStatusBar
{
    Q_OBJECT

public:
    explicit Statusbar(QWidget *parent = nullptr);

    void saveState(QSettings &settings) const;
    void loadState(const QSettings &settings);

    bool showDev() const;
    bool showKev() const;
    bool showCmd() const;
    bool showRes() const;
    int resSize() const;

    CellConf cellConf() const;

signals:
    void confChanged();

public slots:
    void onMouseMove(const QPoint &p);
    void onMouseTap(const QPoint &p);
    void onMouseSwipe(const QPoint &p1, const QPoint &p2, qint64 d);

private:
    QLineEdit *m_moveOut{};
    QLineEdit *m_tapOut{};
    QLineEdit *m_swipeOut{};

    QCheckBox *m_devFlag{};
    QCheckBox *m_kevFlag{};
    QCheckBox *m_cmdFlag{};
    QCheckBox *m_resFlag{};
    QSpinBox *m_resSizeInp{};
};

#endif // STATUSBAR_H
