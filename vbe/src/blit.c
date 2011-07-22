#include <blit.h>
#include <lib.h>

void blit_generic(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst)
{
    for(int yPos = 0; yPos < height; yPos++)
    {
        for(int xPos = 0; xPos < width; xPos++)
        {
            uint32_t srcColor = vid_getPixel(xPos + offset_x, yPos + offset_y, src);
        
            uint8_t r, g, b, a;
            vid_unmapRGBA(srcColor, &r, &g, &b, &a, src);
        
            uint32_t dstColor = vid_mapRGBA(r, g, b, a, dst);
        
            vid_setPixel(dstColor, x + xPos, y + yPos, dst);
        }
    }
}

void blit_blockcopy(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst)
{
    uint8_t bytesPerPixel = src->bytesPerPixel;
    
    for(int yPos = 0; yPos < height; yPos++)
    {
        uint8_t* srcptr = vid_getFramebufferPositionPointer(offset_x, yPos + offset_y, src);
        uint8_t* dstptr = vid_getFramebufferPositionPointer(x, y + yPos, dst);

        memcpy (dstptr, srcptr, width * bytesPerPixel);
    }
}

/*void blit_RGBX8888toBGRX8888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst)
{
    for(int yPos = 0; yPos < height; yPos++)
    {
        uint32_t* srcptr = (uint32_t*)vid_getFramebufferPositionPointer(offset_x, offset_y + yPos, src);
        uint8_t* dstptr = (uint8_t*)vid_getFramebufferPositionPointer(x, y + yPos, dst);
       
        for(int xPos = 0; xPos < width; xPos++)
        {
            uint32_t srccolor = *srcptr++;
            
            uint8_t sr = ((srccolor >> 24) & 0xFF);
            uint8_t sg = ((srccolor >> 16) & 0xFF);
            uint8_t sb = ((srccolor >> 8) & 0xFF);
            uint8_t sa = ((srccolor >> 0) & 0xFF);
            
            *dstptr++ = sa;
            *dstptr++ = sr;
            *dstptr++ = sg;
            *dstptr++ = sb;
        }
    }
}

void blit_RGBX8888toBGR888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst)
{
    for(int yPos = 0; yPos < height; yPos++)
    {
        uint32_t* srcptr = (uint32_t*)vid_getFramebufferPositionPointer(offset_x, offset_y + yPos, src);
        uint8_t* dstptr = (uint8_t*)vid_getFramebufferPositionPointer(x, y + yPos, dst);
        
        for(int xPos = 0; xPos < width; xPos++)
        {
            uint32_t srccolor = *srcptr++;
            
            uint8_t sr = ((srccolor >> 24) & 0xFF);
            uint8_t sg = ((srccolor >> 16) & 0xFF);
            uint8_t sb = ((srccolor >> 8) & 0xFF);
            
            *dstptr++ = sr;
            *dstptr++ = sg;
            *dstptr++ = sb;
        }
    }
}

void blit_RGBX8888toRGB888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst)
{
    for(int yPos = 0; yPos < height; yPos++)
    {
        uint32_t* srcptr = (uint32_t*)vid_getFramebufferPositionPointer(offset_x, offset_y + yPos, src);
        uint8_t* dstptr = (uint8_t*)vid_getFramebufferPositionPointer(x, y + yPos, dst);
        
        for(int xPos = 0; xPos < width; xPos++)
        {
            uint32_t srccolor = *srcptr++;
            
            uint8_t sr = ((srccolor >> 24) & 0xFF);
            uint8_t sg = ((srccolor >> 16) & 0xFF);
            uint8_t sb = ((srccolor >> 8) & 0xFF);
            
            *dstptr++ = sb;
            *dstptr++ = sg;
            *dstptr++ = sr;
        }
    }
}

void blit_RGBX8888toXRGB8888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst)
{
    for(int yPos = 0; yPos < height; yPos++)
    {
        uint32_t* srcptr = (uint32_t*)vid_getFramebufferPositionPointer(offset_x, offset_y + yPos, src);
        uint8_t* dstptr = (uint8_t*)vid_getFramebufferPositionPointer(x, y + yPos, dst);
        
        for(int xPos = 0; xPos < width; xPos++)
        {
            uint32_t srccolor = *srcptr++;
            
            *dstptr++ = ((srccolor >> 8) & 0xFF);
            *dstptr++ = ((srccolor >> 16) & 0xFF);
            *dstptr++ = ((srccolor >> 24) & 0xFF);
            *dstptr++ = ((srccolor >> 0) & 0xFF);
        }
    }
}

void blit_RGBX8888toXBGR8888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst)
{
    for(int yPos = 0; yPos < height; yPos++)
    {
        uint32_t* srcptr = (uint32_t*)vid_getFramebufferPositionPointer(offset_x, offset_y + yPos, src);
        uint8_t* dstptr = (uint8_t*)vid_getFramebufferPositionPointer(x, y + yPos, dst);
        
        for(int xPos = 0; xPos < width; xPos++)
        {
            uint32_t srccolor = *srcptr++;
            
            uint8_t sr = ((srccolor >> 24) & 0xFF);
            uint8_t sg = ((srccolor >> 16) & 0xFF);
            uint8_t sb = ((srccolor >> 8) & 0xFF);
            uint8_t sa = ((srccolor >> 0) & 0xFF);
            
            *dstptr++ = sr;
            *dstptr++ = sg;
            *dstptr++ = sb;
            *dstptr++ = sa;
        }
    }
}

void blit_RGB888toBGRX8888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst)
{
    uint8_t* srcptr = (uint8_t*)vid_getFramebufferPositionPointer(offset_x, offset_y, src);
    uint8_t* dstptr = (uint8_t*)vid_getFramebufferPositionPointer(x, y, dst);
    
    for(int yPos = 0; yPos < height; yPos++)
    {
        for(int xPos = 0; xPos < width; xPos++)
        {
            uint8_t r = *srcptr++;
            uint8_t g = *srcptr++;
            uint8_t b = *srcptr++;
            
            *dstptr++ = 255;    //alpha
            *dstptr++ = r;
            *dstptr++ = g;
            *dstptr++ = b;
        }
        
        srcptr += src->bytesPerLine - src->bytesPerPixel * width;
        dstptr += dst->bytesPerLine - dst->bytesPerPixel * width;
    }
}

void blit_RGB888toRGBX8888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst)
{
    uint8_t* srcptr = (uint8_t*)vid_getFramebufferPositionPointer(offset_x, offset_y, src);
    uint8_t* dstptr = (uint8_t*)vid_getFramebufferPositionPointer(x, y, dst);
    
    for(int yPos = 0; yPos < height; yPos++)
    {
        for(int xPos = 0; xPos < width; xPos++)
        {
            uint8_t r = *srcptr++;
            uint8_t g = *srcptr++;
            uint8_t b = *srcptr++;
            
            *dstptr++ = r;
            *dstptr++ = g;
            *dstptr++ = b;
            *dstptr++ = 255;    //alpha
        }
        
        srcptr += src->bytesPerLine - src->bytesPerPixel * width;
        dstptr += dst->bytesPerLine - dst->bytesPerPixel * width;
    }
}

void blit_RGB888toBGR888(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst)
{
    uint8_t* srcptr = (uint8_t*)vid_getFramebufferPositionPointer(offset_x, offset_y, src);
    uint8_t* dstptr = (uint8_t*)vid_getFramebufferPositionPointer(x, y, dst);
    
    for(int yPos = 0; yPos < height; yPos++)
    {
        for(int xPos = 0; xPos < width; xPos++)
        {
            uint8_t r = *srcptr++;
            uint8_t g = *srcptr++;
            uint8_t b = *srcptr++;
            
            *dstptr++ = b;
            *dstptr++ = g;
            *dstptr++ = r;
        }
        
        srcptr += src->bytesPerLine - src->bytesPerPixel * width;
        dstptr += dst->bytesPerLine - dst->bytesPerPixel * width;
    }
}*/
