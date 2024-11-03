#ifndef TOOLBAR_H
#define TOOLBAR_H
#include <QToolBar>
#include "cellwidget.h"

class QPushButton;
class QSpinBox;
class QLabel;
class QLineEdit;
class QCheckBox;
class QSettings;

class Toolbar : public QToolBar
{
    Q_OBJECT

signals:
    void runStateChanged(int state);
    void applyConfRequested();

public:
    Toolbar(QWidget *parent = nullptr);
    ~Toolbar();

    int runState() const;
    QString host() const;
    int port() const;
    int rows() const;
    int cols() const;
    int scale() const;
    int rate() const;
    bool fast() const;

    CellWidgetConf cellConf() const;

    void saveState(QSettings &settings) const;
    void loadState(const QSettings &settings);

private:
    QCheckBox *m_runInp{};
    QPushButton *m_applyBtn{};
    QLineEdit *m_hostInp{};
    QSpinBox *m_portInp{};
    QSpinBox *m_rowsInp{};
    QSpinBox *m_colsInp{};
    QSpinBox *m_scaleInp{};
    QSpinBox *m_rateInp{};
    QCheckBox *m_fastInp{};
};

#endif // TOOLBAR_H
