/*
   DivvyDroid - Application to screencast and remote control Android devices.

   Copyright (C) 2019 - Mladen Milinkovic <maxrd2@smoothware.net>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QThread>

#include "adbclient.h"

struct AVFormatContext;
struct AVStream;
struct AVCodecContext;
struct SwsContext;
struct AVFrame;

class VideoThread : public QThread
{
	Q_OBJECT

public:
    enum VideMode { FastH264, NativeJpg, NativePng, NativeRaw };

    VideoThread(QObject *parent = nullptr);
    ~VideoThread();

    void setDeviceId(const QString &deviceId);
    void setVideoMode(VideMode mode);
    void setImageSize(int w, int h);
    void setNativeInterval(unsigned long ms);

signals:
    void imageReady(const QImage &image);

protected:
	void run();

private:
	const char * h264Error(int errorCode);
	bool h264Connect();
	bool h264Init();
	int h264VideoStreamIndex();
    bool h264Loop();
    void h264Exit();

    void nativeLoop();

    QString m_deviceId{};
    VideMode m_videoMode{FastH264};
    int m_imageWidth{426};
    int m_imageHeight{240};
    unsigned long m_nativeInterval{100};

    AdbClient *m_adb{};
    AVFormatContext *m_avFormat{};
    AVStream *m_avStream{};
    AVCodecContext *m_codecCtx{};
    SwsContext *m_swsContext{};
    AVFrame *m_frame{};
    AVFrame *m_rgbFrame{};
    bool m_h264Active{};
};

#endif // VIDEOTHREAD_H
