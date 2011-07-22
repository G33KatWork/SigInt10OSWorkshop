#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <types.h>
#include <vbe.h>

enum video_blit_format
{
    //normal rgba format
    VIDEO_BLIT_FORMAT_RGBA,
    
    //normal rgb format
    VIDEO_BLIT_FORMAT_RGB,

    //optimized rgba formats
    VIDEO_BLIT_FORMAT_ARGB_8888,
    VIDEO_BLIT_FORMAT_ABGR_8888,
    VIDEO_BLIT_FORMAT_RGBA_8888,
    VIDEO_BLIT_FORMAT_BGRA_8888,

    //optimized rgb formats
    VIDEO_BLIT_FORMAT_RGB_888,
    VIDEO_BLIT_FORMAT_BGR_888,
    VIDEO_BLIT_FORMAT_RGB_565,
    VIDEO_BLIT_FORMAT_BGR_565,
    VIDEO_BLIT_FORMAT_BGR_555,
    VIDEO_BLIT_FORMAT_RGB_555,

    //indexed color using a palette
    VIDEO_BLIT_FORMAT_INDEXCOLOR,
};

enum video_mode_type
{
    VIDEO_MODE_TYPE_RGB,
    VIDEO_MODE_TYPE_INDEXED,
    VIDEO_MODE_TYPE_UNKNOWN,
};

struct blit_surface
{
    uint32_t width, height;
    uint8_t bpp;
    uint8_t bytesPerPixel;
    
    enum video_mode_type modeType;
    
    uint8_t redMaskSize, redPosition;
    uint8_t greenMaskSize, greenPosition;
    uint8_t blueMaskSize, bluePosition;
    uint8_t rsvMaskSize, rsvPosition;
    
    uint32_t bytesPerLine;
    
    uint8_t page;
    
    uint8_t* data;
};

void vid_getBlitSurfaceFromVBEMode(struct vbeModeInfo* modeInfo, struct blit_surface* surface);

enum video_blit_format vid_getFramebufferBlitFormat(struct blit_surface* surface);

uint32_t vid_mapRGB(uint8_t red, uint8_t green, uint8_t blue, struct blit_surface* surface);
uint32_t vid_mapRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, struct blit_surface* surface);
void vid_unmapRGBA(uint32_t color, uint8_t* red, uint8_t* green, uint8_t* blue, uint8_t* alpha, struct blit_surface* surface);

uint8_t* vid_getFramebufferPositionPointer(uint32_t x, uint32_t y, struct blit_surface* surface);

uint32_t vid_getPixel(int x, int y, struct blit_surface* surface);
void vid_setPixel(uint32_t color, int x, int y, struct blit_surface* surface);

//color needs to be in a format the framebuffer expects
void vid_fillRect(uint32_t color, int x, int y, int width, int height, struct blit_surface* surface);

void vid_blit(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst);

#endif
