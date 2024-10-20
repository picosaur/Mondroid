#ifndef TOOLBAR_H
#define TOOLBAR_H
#include <QToolBar>

class QPushButton;
class QSpinBox;
class QLabel;
class QLineEdit;

class Toolbar : public QToolBar
{
    Q_OBJECT

signals:
    void start();
    void stop();

public:
    Toolbar(QWidget *parent = nullptr);
    ~Toolbar();

    int rows() const;
    int cols() const;
    int scale() const;
    QString host() const;
    int port() const;

private:
    QPushButton *m_startBtn{};
    QPushButton *m_stopBtn{};
    QSpinBox *m_rowsInp{};
    QSpinBox *m_colsInp{};
    QSpinBox *m_scaleInp{};
    QLineEdit *m_hostInp{};
    QSpinBox *m_portInp{};
};

#endif // TOOLBAR_H
