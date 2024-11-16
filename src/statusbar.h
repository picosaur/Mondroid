#ifndef STATUSBAR_H
#define STATUSBAR_H
#include <QSettings>
#include <QStatusBar>

class QLineEdit;
class QCheckBox;

class Statusbar : public QStatusBar
{
    Q_OBJECT

public:
    explicit Statusbar(QWidget *parent = nullptr);

    void saveState(QSettings &settings) const;
    void loadState(const QSettings &settings);

    bool showDevInp() const;
    bool showKevInp() const;
    bool showCmdInp() const;

signals:
    void showDevChanged(bool);
    void showKevChanged(bool);
    void showCmdChanged(bool);

public slots:
    void onMouseMove(const QPoint &p);
    void onMouseTap(const QPoint &p);
    void onMouseSwipe(const QPoint &p1, const QPoint &p2, qint64 d);

private slots:
    void onDevFlagStateChanged(int state);
    void onKevFlagStateChanged(int state);
    void onCmdFlagStateChanged(int state);

private:
    QLineEdit *m_moveOut{};
    QLineEdit *m_tapOut{};
    QLineEdit *m_swipeOut{};

    QCheckBox *m_devFlag{};
    QCheckBox *m_kevFlag{};
    QCheckBox *m_cmdFlag{};
};

#endif // STATUSBAR_H
