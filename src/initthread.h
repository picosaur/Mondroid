#ifndef INITTHREAD_H
#define INITTHREAD_H
#include <QThread>

#define IMAGE_WIDTH 360

class InitThread : public QThread
{
	Q_OBJECT

public:
	InitThread(QObject *parent = nullptr);
	~InitThread();

signals:
	void deviceConnected();
	void inputReady();

protected:
    void run();
};

#endif // INITTHREAD_H
