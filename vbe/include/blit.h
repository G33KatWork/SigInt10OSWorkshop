#ifndef _BLIT_H_
#define _BLIT_H_

#include <video.h>

void blit_generic(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst);
void blit_blockcopy(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst);

//the following optimized blitters are the most needed.
//either the bitmap is 24 bit (RGB) or 32 bit (RGBX).
//All other formats need to be processed with a blockcopy if possible or with the generic blitter

//optimized blitter for RGBX8888 to different destination formats
/*void blit_RGBX8888toBGRX8888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst);
void blit_RGBX8888toBGR888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst);
void blit_RGBX8888toRGB888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst);
void blit_RGBX8888toXRGB8888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst);
void blit_RGBX8888toXBGR8888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst);

//optimized blitter for RGB888 to different destination formats
void blit_RGB888toBGRX8888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst);
void blit_RGB888toRGBX8888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst);
void blit_RGB888toBGR888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst);*/

#endif

//XRGB8888 -> RGB888