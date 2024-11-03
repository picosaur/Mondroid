#ifndef CELLWIDGET_H
#define CELLWIDGET_H
#include <QWidget>
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
class QLineEdit;
class QPushButton;
class VideoThread;

struct CellWidgetConf
{
    QString host{};
    int port{};
    int rows{};
    int cols{};
    int scale{};
    int rate{};
    bool fast{};
};

class CellWidget : public QWidget
{
    Q_OBJECT

public:
    CellWidget(QWidget *parent = nullptr);
    ~CellWidget();

    void setDevice(const QString &deviceId);
    void setConf(const CellWidgetConf &conf);

    QString device() const;
    const CellWidgetConf &conf() const;

    void start();
    void stop();

public slots:
    void updateScreen(const QImage &image);

private slots:
    void onVideoFinished();
    void onABtnClicked();
    void onBBtnClicked();
    void onCBtnClicked();

private:
    CellWidgetConf m_conf{};

    QVBoxLayout *m_mainLayout{};
    QHBoxLayout *m_toolLayout{};
    QScrollArea *m_area{};
    QLabel *m_screen{};
    QLineEdit *m_deviceInp{};
    QPushButton *m_aBtn{}, *m_bBtn{}, *m_cBtn{};

    VideoThread *m_videoThread{};
};
#endif // CELLWIDGET_H
