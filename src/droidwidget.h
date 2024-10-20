#ifndef DROIDWIDGET_H
#define DROIDWIDGET_H
#include <QWidget>
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
class QLineEdit;
class QPushButton;
class InitThread;
class VideoThread;

class DroidWidget : public QWidget
{
    Q_OBJECT

public:
    DroidWidget(QWidget *parent = nullptr);
    ~DroidWidget();

    void setDeviceId(const QString &deviceId);
    void start();
    void stop();

    void updateScreen(const QImage &image);

private slots:
    void onVideoFinished();
    void onABtnClicked();
    void onBBtnClicked();
    void onCBtnClicked();

private:
    QVBoxLayout *m_mainLayout{};
    QHBoxLayout *m_toolLayout{};
    QScrollArea *m_area{};
    QLabel *m_screen{};
    QLineEdit *m_deviceInp{};
    QPushButton *m_aBtn{}, *m_bBtn{}, *m_cBtn{};

    InitThread *m_initThread{};
    VideoThread *m_videoThread{};
};
#endif // DROIDWIDGET_H
