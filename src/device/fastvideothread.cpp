// Based on DivvyDroid by Mladen Milinkovic <maxrd2@smoothware.net>
// https://github.com/maxrd2/DivvyDroid
// See also
// https://stackoverflow.com/questions/34511312/how-to-encode-a-video-from-several-images-generated-in-a-c-program-without-wri
// https://ffmpeg.org/doxygen/trunk/encode_video_8c-example.html
// https://github.com/apc-llc/moviemaker-cpp/blob/master/src/writer.cpp
// https://github.com/libav/libav/blob/master/doc/examples/decode_video.c
// https://github.com/spacecowboy/NotePad/blob/c3ec24e1aca5979ef8273d8ddc48cc637518de0b/github_on_emu_started.sh#L54

#include "fastvideothread.h"
#include "adbclient.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
#include <libswscale/swscale.h>
}

FastVideoThread::FastVideoThread(QObject *parent)
    : VideoThread{parent}
{}

FastVideoThread::~FastVideoThread() {}

void FastVideoThread::loop()
{
    if (!initStream()) {
        exitStream();
        return;
    }

    int streamIndex = getStreamIndex();
    if (streamIndex == -1) {
        exitStream();
        return;
    }

    if (!initFrames()) {
        return;
    }

    AVPacket pkt;
    pkt.data = nullptr;
    pkt.size = 0;

    while (!isInterruptionRequested()) {
        // read frame
        if (int ret = av_read_frame(m_avFormat, &pkt); ret < 0) {
            qWarning() << __FUNCTION__ << "av_read_frame failed:" << streamError(ret);
            msleep(1000);
            continue;
        }

        // check stream index
        if (pkt.stream_index != streamIndex) {
            continue;
        }

        // decode packet
        decodePacket(&pkt);
        av_packet_unref(&pkt);
    }

    exitStream();
}

bool FastVideoThread::initStream()
{
    const int bufSize = 8192;
    unsigned char *buf = reinterpret_cast<unsigned char *>(av_malloc(bufSize));

    // AVFormatContext
    m_avFormat = avformat_alloc_context();
    Q_ASSERT(m_avFormat != nullptr);
    m_avFormat->pb = avio_alloc_context(buf, bufSize, 0, this, readPacket, nullptr, nullptr);
    Q_ASSERT(m_avFormat->pb != nullptr);

    int ret{};
    if ((ret = avformat_open_input(&m_avFormat, nullptr, nullptr, nullptr)) < 0) {
        qDebug() << __FUNCTION__ << "can't open input:" << streamError(ret);
        return false;
    }

    m_avFormat->probesize = 32;
    //	m_avFormat->max_analyze_duration = 0;
    if ((ret = avformat_find_stream_info(m_avFormat, nullptr)) < 0) {
        qDebug() << __FUNCTION__ << "can't find stream information:" << streamError(ret);
        return false;
    }
    av_dump_format(m_avFormat, 0, "", 0);

    return true;
}

bool FastVideoThread::initFrames()
{
    // AVFrame
    m_frame = av_frame_alloc();
    if (m_frame == nullptr) {
        return false;
    }

    // AVFrame RGB
    m_rgbFrame = av_frame_alloc();
    if (m_rgbFrame == nullptr) {
        return false;
    }

    av_image_alloc(m_rgbFrame->data,
                   m_rgbFrame->linesize,
                   getScaledSize(m_codecCtx->width),
                   getScaledSize(m_codecCtx->height),
                   AV_PIX_FMT_RGB24,
                   32);

    // SWS
    m_swsContext = sws_getContext(m_codecCtx->width,
                                  m_codecCtx->height,
                                  m_codecCtx->pix_fmt,
                                  getScaledSize(m_codecCtx->width),
                                  getScaledSize(m_codecCtx->height),
                                  AV_PIX_FMT_RGB24,
                                  SWS_BICUBIC,
                                  nullptr,
                                  nullptr,
                                  nullptr);
    if (m_swsContext == nullptr) {
        return false;
    }

    return true;
}

int FastVideoThread::getStreamIndex()
{
    int streamIndex{-1};
    int ret{};

    for (unsigned int i = 0; i < m_avFormat->nb_streams; i++) {
        m_avStream = m_avFormat->streams[i];
        if (m_avStream->codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
            continue;

        const AVCodec *dec = avcodec_find_decoder(m_avStream->codecpar->codec_id);
        if (!dec) {
            qDebug() << __FUNCTION__ << "can't find decoder for stream" << i;
            continue;
        }

        m_codecCtx = avcodec_alloc_context3(dec);
        if (!m_codecCtx) {
            qDebug() << __FUNCTION__ << "can't allocate the decoder context for stream" << i;
            continue;
        }
        ret = avcodec_parameters_to_context(m_codecCtx, m_avStream->codecpar);
        if (ret < 0) {
            qDebug() << __FUNCTION__
                     << "failed to copy decoder parameters to input "
                        "decoder context "
                        "for stream"
                     << i << streamError(ret);
            avcodec_free_context(&m_codecCtx);
            continue;
        }
        if (m_codecCtx->codec_type != AVMEDIA_TYPE_VIDEO) {
            avcodec_free_context(&m_codecCtx);
            continue;
        }
        ret = avcodec_open2(m_codecCtx, dec, nullptr);
        if (ret < 0) {
            qDebug() << __FUNCTION__ << "failed to open decoder for stream" << i
                     << streamError(ret);
            avcodec_free_context(&m_codecCtx);
            continue;
        }

        streamIndex = i;
        break;
    }

    return streamIndex;
}

void FastVideoThread::exitStream()
{
    if (m_swsContext) {
        sws_freeContext(m_swsContext);
    }
    if (m_frame) {
        av_frame_free(&m_frame);
    }
    if (m_rgbFrame) {
        av_freep(&m_rgbFrame->data[0]);
        av_frame_free(&m_rgbFrame);
    }
    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
    }
    if (m_avFormat) {
        AVIOContext *ioContext = m_avFormat->pb;
        avformat_close_input(&m_avFormat);
        avio_context_free(&ioContext);
    }
}

void FastVideoThread::decodePacket(AVPacket *pkt)
{
    int ret{};

    // send packet
    ret = avcodec_send_packet(m_codecCtx, pkt);
    if (ret < 0) {
        qWarning() << __FUNCTION__ << "Error sending a packet for decoding";
    }

    while (ret >= 0) {
        // receiver frame
        ret = avcodec_receive_frame(m_codecCtx, m_frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        } else if (ret < 0) {
            qWarning() << __FUNCTION__ << "Error during decoding";
            return;
        }
        // scale
        sws_scale(m_swsContext,
                  m_frame->data,
                  m_frame->linesize,
                  0,
                  m_codecCtx->height,
                  m_rgbFrame->data,
                  m_rgbFrame->linesize);
        // image
        QImage img(getScaledSize(m_codecCtx->width),
                   getScaledSize(m_codecCtx->height),
                   QImage::Format_RGB888);
        const uint8_t *data = m_rgbFrame->data[0];
        for (int y = 0; y < img.height(); ++y) {
            memcpy(img.scanLine(y), data, img.bytesPerLine());
            data += m_rgbFrame->linesize[0];
        }
        emit imageReady(img);
    }
}

int FastVideoThread::readPacket(void *u, uint8_t *buf, int buf_size)
{
    auto *self = reinterpret_cast<FastVideoThread *>(u);

    if (!self->m_streamFlag) {
        self->adb()->startVideoStream(30);
        self->m_streamFlag = true;
    }

    qint64 len{};
    while (len == 0) {
        if (self->isInterruptionRequested()) {
            len = -1;
            break;
        }
        len = self->adb()->bytesAvailable();
        if (len > 0) {
            break;
        }
        if (self->adb()->waitForReadyRead(50)) {
            len = self->adb()->bytesAvailable();
            break;
        }
        if (!self->adb()->isConnected()) {
            self->m_streamFlag = {};
            break;
        }
        if (auto err = self->adb()->error(); err != QTcpSocket::SocketTimeoutError) {
            qWarning() << __FUNCTION__ << "read failed: " << err;
            break;
        }
    }
    if (len > buf_size) {
        len = buf_size;
    }
    if (len > 0 && !self->adb()->read(buf, len)) {
        len = -1;
    }
    return len;
}

const char *FastVideoThread::streamError(int errorCode)
{
    static char errorText[1024];
    av_strerror(errorCode, errorText, sizeof(errorText));
    return errorText;
}
