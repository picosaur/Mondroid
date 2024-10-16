#ifndef DROIDWIDGET_H
#define DROIDWIDGET_H
#include <QWidget>
class QLabel;
class QVBoxLayout;
class QScrollArea;
class QLineEdit;
class InitThread;
class VideoThread;

class DroidWidget : public QWidget
{
    Q_OBJECT

public:
    DroidWidget(QWidget *parent = nullptr);
    ~DroidWidget();

    void start();
    void stop();

public slots:
    void onDeviceReady();
    void onInputReady();
    void updateScreen(const QImage &image);

private:
    QVBoxLayout *m_layout{};
    QScrollArea *m_area{};
    QLabel *m_screen{};
    QLineEdit *m_hostInp{};

    InitThread *m_initThread{};
    VideoThread *m_videoThread{};
};
#endif // DROIDWIDGET_H
