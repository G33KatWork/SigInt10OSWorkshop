#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <types.h>
#include <video.h>

// for detailed information about the bitmap format see http://en.wikipedia.org/wiki/BMP_file_format

struct bitmap_file_hdr_s
{
    unsigned char magic[2];
    uint32_t filesize;
    uint16_t res1;
    uint16_t res2;
    uint32_t bmp_offset;
} __attribute__ ((packed));
typedef struct bitmap_file_hdr_s bitmap_file_hdr;

typedef enum
{
    BITMAP_COMP_RGB = 0,
    BITMAP_COMP_RLE8 = 1,
    BITMAP_COMP_RLE4 = 2,
    BITMAP_COMP_BITFIELDS = 3,
    BITMAP_COMP_JPEG = 4,
    BITMAP_COMP_PNG = 5
} bitmap_compression_type;

struct bitmap_info_hdr_s {
    uint32_t headersize;
    int32_t width;
    int32_t height;
    uint16_t nplanes;
    uint16_t bitspp;
    uint32_t compression_type;
    uint32_t bmp_bytes;
    int32_t hres;
    int32_t vres;
    uint32_t ncolors;
    uint32_t nimpcolors;
    uint32_t redmask;
    uint32_t greenmask;
    uint32_t bluemask;
    uint32_t alphamask;
} __attribute__ ((packed));
typedef struct bitmap_info_hdr_s bitmap_info_hdr;


static inline const bitmap_file_hdr* bitmap_get_file_header(const uint8_t* bitmapdata)
{
	return (const bitmap_file_hdr*)bitmapdata;
}

static inline const bitmap_info_hdr* bitmap_get_info_header(const uint8_t* bitmapdata)
{
    return (const bitmap_info_hdr*)(bitmapdata + sizeof(bitmap_file_hdr));
}


int bitmap_check_support(const uint8_t* bitmapdata);
void bitmap_get_blit_surface(const uint8_t* bitmapdata, struct blit_surface* surface);

#endif
