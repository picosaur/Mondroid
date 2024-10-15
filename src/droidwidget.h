#ifndef DROIDWIDGET_H
#define DROIDWIDGET_H
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include "device/videothread.h"
#include "initthread.h"
#include <qscrollarea.h>

class DroidWidget : public QWidget
{
    Q_OBJECT

public:
    DroidWidget(QWidget *parent = nullptr);
    ~DroidWidget();

    void init();

public slots:
    void onDeviceReady();
    void onInputReady();
    void updateScreen(const QImage &image);

private:
    QVBoxLayout *m_layout{};
    QScrollArea *m_area{};
    QLabel *m_screen{};
    InitThread *m_initThread{};
    VideoThread *m_videoThread{};
};
#endif // DROIDWIDGET_H
