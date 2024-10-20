#ifndef FRAMEBUFINFO_H
#define FRAMEBUFINFO_H
#include <QImage>
#include <QtGlobal>

#define FB_VAR(VAR) (version == 16 ? v0.VAR : version == 1 ? v1.VAR : version == 2 ? v2.VAR : 0)
#define FB_VAR_1(VAR, DEF) (version == 16 ? DEF : version == 1 ? v1.VAR : version == 2 ? v2.VAR : 0)

struct FramebufInfo
{
    quint32 version;
    union {
        struct
        {
            quint32 size{};
            quint32 width{};
            quint32 height{};
        } v0; // version=16 here which is actually 16bpp
        struct
        {
            quint32 bpp{};
            quint32 size{};
            quint32 width{};
            quint32 height{};
            quint32 red_offset{};
            quint32 red_length{};
            quint32 blue_offset{};
            quint32 blue_length{};
            quint32 green_offset{};
            quint32 green_length{};
            quint32 alpha_offset{};
            quint32 alpha_length{};
        } v1;
        struct
        {
            quint32 bpp{};
            quint32 colorSpace{};
            quint32 size{};
            quint32 width{};
            quint32 height{};
            quint32 red_offset{};
            quint32 red_length{};
            quint32 blue_offset{};
            quint32 blue_length{};
            quint32 green_offset{};
            quint32 green_length{};
            quint32 alpha_offset{};
            quint32 alpha_length{};
        } v2;
    };

    quint32 width() const { return FB_VAR(width); }
    quint32 height() const { return FB_VAR(height); }
    quint32 size() const { return FB_VAR(size); }
    quint32 bpp() const { return FB_VAR_1(bpp, 16); }

    QImage::Format format()
    {
        if (version == 16)
            return QImage::Format_RGB16;
        //			if(version == 1 && v1.bpp == 24)
        //				return QImage::Format_RGB888;
        //			if(version <= 2 && v1.bpp == 32) // TODO: consider offsets and lengths to figure the format
        return QImage::Format_RGB32;
    }

    bool valid;
};

#endif // FRAMEBUFINFO_H
