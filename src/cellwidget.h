#ifndef CELLWIDGET_H
#define CELLWIDGET_H
#include <QLabel>
#include <QMouseEvent>
#include <QWidget>
#include <qelapsedtimer.h>

class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
class QLineEdit;
class QPushButton;
class QSpinBox;

class VideoThread;
class AdbClient;

class ScreenLabel : public QLabel
{
    Q_OBJECT

    QPointF m_p1{}, m_p2{};
    QElapsedTimer m_timer;

public:
    ScreenLabel() { setMouseTracking(true); }
    ~ScreenLabel() {}

signals:
    void mouseMove(QPointF point);
    void mouseTap(QPointF point);
    void mouseSwipe(QPointF p1, QPointF p2, qint64 dur);

protected:
    void mouseMoveEvent(QMouseEvent *event) override { emit mouseMove(event->localPos()); }

    void mousePressEvent(QMouseEvent *event) override
    {
        m_p1 = event->localPos();
        m_timer.start();
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        m_p2 = event->localPos();
        auto d = m_timer.elapsed();
        if (d < 100) {
            emit mouseTap(m_p1);
        } else {
            emit mouseSwipe(m_p1, m_p2, d);
        }
    }
};

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

signals:
    void mouseMove(QPoint);
    void mouseTap(QPoint);
    void mouseSwipe(QPoint, QPoint, qint64);

public slots:
    void updateScreen(const QImage &image);

private slots:
    void onVideoFinished();
    void onKeBtnClicked();
    void onMouseMove(const QPointF &p);
    void onMouseTap(const QPointF &p);
    void onMouseSwipe(const QPointF &ps, const QPointF &pe, qint64 d);

private:
    QPoint toPoint(const QPointF &p) const;

private:
    CellWidgetConf m_conf{};
    AdbClient *m_adb{};
    VideoThread *m_videoThread{};

    QVBoxLayout *m_mainLayout{};
    QHBoxLayout *m_toolLayout{};
    QScrollArea *m_area{};
    ScreenLabel *m_screen{};
    QLineEdit *m_deviceInp{};
    QPushButton *m_keBtn{};
    QSpinBox *m_keInp{};
};
#endif // CELLWIDGET_H
