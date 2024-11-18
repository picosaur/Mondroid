#ifndef CELLWIDGET_H
#define CELLWIDGET_H
#include <QLabel>
#include <QMouseEvent>
#include <QPointer>
#include <QWidget>
#include <qelapsedtimer.h>

class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QPlainTextEdit;
class VideoThread;
class AdbClient;

// ScreenLabel
// ----------------------------------------------------------------------------
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

// GridConf
// ----------------------------------------------------------------------------
struct GridConf
{
    bool discover{};
    QString host{};
    int port{};
    int rows{};
    int cols{};
    int rate{};
    bool fast{};
    int scale{};

    bool isEqualExceptScale(const GridConf &other) const
    {
        return this->discover == other.discover && this->port == other.port
               && this->rows == other.rows && this->cols == other.cols && this->rate == other.rate
               && this->fast == other.fast && this->scale != other.scale;
    }
};

// CellConf
// ----------------------------------------------------------------------------
struct CellConf
{
    bool devVisible{};
    bool kevVisible{};
    bool cmdVisible{};
    bool resVisible{};
    int resOutSize{};
};

// CellWidget
// ----------------------------------------------------------------------------
class CellWidget : public QWidget
{
    Q_OBJECT

public:
    CellWidget(QWidget *parent = nullptr);
    ~CellWidget();

    void setDevice(const QString &deviceId);
    QString device() const;

    void setGridConf(const GridConf &conf);
    const GridConf &gridConf() const;

    void setCellConf(const CellConf &conf);
    const CellConf &cellConf() const;

    void setImageScale(int scale);

    void start();
    void stop();

signals:
    void mouseMove(QPoint);
    void mouseTap(QPoint);
    void mouseSwipe(QPoint, QPoint, qint64);

private slots:
    void onUpdateScreen(const QImage &image);
    void onVideoFinished();
    void onKevBtnClicked();
    void onCmdBtnClicked();
    void onMouseMove(const QPointF &p);
    void onMouseTap(const QPointF &p);
    void onMouseSwipe(const QPointF &ps, const QPointF &pe, qint64 d);

private:
    void setDevVisible(bool v);
    void setKevVisible(bool v);
    void setCmdVisible(bool v);
    void setResVisible(bool v);
    void setResOutSize(int sz);
    QPoint toPoint(const QPointF &p) const;

private:
    GridConf m_gridConf{};
    CellConf m_cellConf{};

    QPointer<AdbClient> m_adb;
    QPointer<VideoThread> m_videoThread;

    QVBoxLayout *m_mainLayout{};
    QScrollArea *m_area{};
    ScreenLabel *m_screen{};

    QHBoxLayout *m_devLayout{};
    QLabel *m_devLabel{};
    QLineEdit *m_devInp{};

    QHBoxLayout *m_kevLayout{};
    QLabel *m_kevLabel{};
    QPushButton *m_kevBtn{};
    QSpinBox *m_kevInp{};

    QHBoxLayout *m_cmdLayout{};
    QLabel *m_cmdLabel{};
    QLineEdit *m_cmdInp{};
    QPushButton *m_cmdBtn{};

    QHBoxLayout *m_resLayout{};
    QLabel *m_resLabel{};
    QPlainTextEdit *m_resOut{};
};
#endif // CELLWIDGET_H
