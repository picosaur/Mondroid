#ifndef INITTHREAD_H
#define INITTHREAD_H
#include <QThread>

#define IMAGE_WIDTH 360

class InitThread : public QThread
{
	Q_OBJECT

public:
	InitThread(QObject *parent = nullptr);
    InitThread(const QString &filter, QObject *parent = nullptr);
    ~InitThread();

signals:
	void deviceConnected();
	void inputReady();

protected:
    void run();

private:
    QString m_filter{};
};

#endif // INITTHREAD_H
