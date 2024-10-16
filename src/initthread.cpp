#include "initthread.h"
#include "device/deviceinfo.h"

#include <QApplication>
#include <QWidget>

InitThread::InitThread(QObject *parent)
	: QThread(parent)
{
}

InitThread::InitThread(const QString &filter, QObject *parent)
    : QThread(parent)
    , m_filter{filter}
{}

InitThread::~InitThread()
{
    requestInterruption();
    wait();
}

void InitThread::run()
{
    DeviceList devices = DeviceInfo::deviceList();
    for (auto it{devices.begin()}; it != devices.end(); ++it) {
        if (it.key().contains(m_filter.toUtf8())) {
            DeviceInfo::connect(it.key());
            emit deviceConnected();
            DeviceInfo::initInput();
            emit inputReady();
            return;
        }
    }
}
