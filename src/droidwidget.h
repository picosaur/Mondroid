#ifndef DROIDWIDGET_H
#define DROIDWIDGET_H
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include "device/videothread.h"
#include "initthread.h"

class DroidWidget : public QWidget
{
public:
    DroidWidget(QWidget *parent = nullptr);
    ~DroidWidget();

    void init();

public slots:
    void onDeviceReady();
    void onInputReady();
    void updateScreen(const QImage &image);

private:
    InitThread *m_initThread{};
    VideoThread *m_videoThread{};
    QVBoxLayout *m_layout{};
    QLabel *m_screen{};
};
#endif // DROIDWIDGET_H
