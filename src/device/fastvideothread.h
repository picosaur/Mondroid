#ifndef FASTVIDEOTHREAD_H
#define FASTVIDEOTHREAD_H

#include "videothread.h"

struct AVFormatContext;
struct AVStream;
struct AVCodecContext;
struct SwsContext;
struct AVFrame;

class AdbClient;

class FastVideoThread : public VideoThread
{
    Q_OBJECT

public:
    explicit FastVideoThread(QObject *parent = nullptr);
    virtual ~FastVideoThread();

private:
    void loop() override final;
    bool initStream();
    int getStreamIndex();
    bool initFrames();
    void exitStream();

    static int readPacket(void *u, uint8_t *buf, int buf_size);
    static const char *streamError(int errorCode);

    bool connectDevice();

    AVFormatContext *m_avFormat{};
    AVStream *m_avStream{};
    AVCodecContext *m_codecCtx{};
    SwsContext *m_swsContext{};
    AVFrame *m_frame{};
    AVFrame *m_rgbFrame{};
};

#endif // FASTVIDEOTHREAD_H
