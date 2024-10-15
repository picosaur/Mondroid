#include "initthread.h"
#include "device/deviceinfo.h"

#include <QApplication>
#include <QWidget>

InitThread::InitThread(QObject *parent)
	: QThread(parent)
{

}

InitThread::~InitThread()
{
    requestInterruption();
    wait();
}

void InitThread::run()
{
    // start adb server and wait for device
    //if(!DeviceInfo::waitForDevice()) {
    //	QApplication::quit();
    //	return;
    //}

    DeviceList devices = DeviceInfo::deviceList();
    DeviceInfo::connect(devices.lastKey());
    //DeviceInfo::connect();

    emit deviceConnected();

    DeviceInfo::initInput();

    emit inputReady();
}
