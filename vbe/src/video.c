#include <video.h>
#include <print.h>
#include <byteswap.h>
#include <blit.h>

void vid_getBlitSurfaceFromVBEMode(struct vbeModeInfo* modeInfo, struct blit_surface* surface)
{
    surface->width = modeInfo->Xres;
    surface->height = modeInfo->Yres;
    surface->bpp = modeInfo->bpp;
    
    switch(modeInfo->memoryModel)
    {
    case VBEMODE_MEMMODEL_PACKED:
        surface->modeType = VIDEO_MODE_TYPE_INDEXED;
        break;
    
    case VBEMODE_MEMMODEL_DIRECTCOL:
        surface->modeType = VIDEO_MODE_TYPE_RGB;
        break;
    
    default:
        surface->modeType = VIDEO_MODE_TYPE_UNKNOWN;
        break;
    }
    
    surface->redMaskSize = modeInfo->redMaskSize;
    surface->redPosition = modeInfo->redPosition;
    surface->greenMaskSize = modeInfo->greenMaskSize;
    surface->greenPosition = modeInfo->greenPosition;
    surface->blueMaskSize = modeInfo->blueMaskSize;
    surface->bluePosition = modeInfo->bluePosition;
    surface->rsvMaskSize = modeInfo->rsvMaskSize;
    surface->rsvPosition = modeInfo->rsvPosition;
    
    surface->bytesPerPixel = (modeInfo->redMaskSize + modeInfo->greenMaskSize + modeInfo->blueMaskSize + modeInfo->rsvMaskSize) / 8;
    
    surface->bytesPerLine = modeInfo->bytesPerScanline;
    
    surface->data = (uint8_t*)modeInfo->physbase;
}


#define OFFSCREEN_BUFFER_LOCATION 0x400000
void vid_createOffscreenSurfaceFromVBEMode(struct vbeModeInfo* modeInfo, struct blit_surface* surface)
{
    //in general we can copy everything, but replace the data buffer pointer with our own
    vid_getBlitSurfaceFromVBEMode(modeInfo, surface);
    
    //FIXME: Use some real allocator here... Even only a placement allocator should suffice, but I'm soooo lazy...
    surface->data = (void*)OFFSCREEN_BUFFER_LOCATION;
}

enum video_blit_format vid_getFramebufferBlitFormat(struct blit_surface* surface)
{
    //32 bit modes
    if(surface->bpp == 32)
    {
        if ((surface->redMaskSize == 8) && (surface->redPosition == 16)
    	&& (surface->greenMaskSize == 8) && (surface->greenPosition == 8)
    	&& (surface->blueMaskSize == 8) && (surface->bluePosition == 0))
        {
            //AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB
            return VIDEO_BLIT_FORMAT_ARGB_8888;
        }
        else if ((surface->redMaskSize == 8) && (surface->redPosition == 0)
    	&& (surface->greenMaskSize == 8) && (surface->greenPosition == 8)
    	&& (surface->blueMaskSize == 8) && (surface->bluePosition == 16))
        {
            //AAAAAAAABBBBBBBBGGGGGGGGRRRRRRRR
            return VIDEO_BLIT_FORMAT_ABGR_8888;
        }
        else if ((surface->redMaskSize == 8) && (surface->redPosition == 24)
    	&& (surface->greenMaskSize == 8) && (surface->greenPosition == 16)
    	&& (surface->blueMaskSize == 8) && (surface->bluePosition == 8))
        {
            //RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA
            return VIDEO_BLIT_FORMAT_RGBA_8888;
        }
        else if ((surface->redMaskSize == 8) && (surface->redPosition == 8)
    	&& (surface->greenMaskSize == 8) && (surface->greenPosition == 16)
    	&& (surface->blueMaskSize == 8) && (surface->bluePosition == 24))
        {
            //RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA
            return VIDEO_BLIT_FORMAT_BGRA_8888;
        }
    }
    
    //24 bit modes
    else if(surface->bpp == 24)
    {
        if ((surface->redMaskSize == 8) && (surface->redPosition == 16)
    	&& (surface->greenMaskSize == 8) && (surface->greenPosition == 8)
    	&& (surface->blueMaskSize == 8) && (surface->bluePosition == 0))
        {
            //RRRRRRRRGGGGGGGGBBBBBBBB
            return VIDEO_BLIT_FORMAT_RGB_888;
        }
        else if ((surface->redMaskSize == 8) && (surface->redPosition == 0)
    	&& (surface->greenMaskSize == 8) && (surface->greenPosition == 8)
    	&& (surface->blueMaskSize == 8) && (surface->bluePosition == 16))
        {
            //BBBBBBBBGGGGGGGGRRRRRRRR
            return VIDEO_BLIT_FORMAT_BGR_888;
        }
    }
    
    //16 bit modes
    else if(surface->bpp == 16)
    {
        if ((surface->redMaskSize == 5) && (surface->redPosition == 11)
    	&& (surface->greenMaskSize == 6) && (surface->greenPosition == 5)
    	&& (surface->blueMaskSize == 5) && (surface->bluePosition == 0))
        {
            //RRRRRGGGGGGBBBBB
            return VIDEO_BLIT_FORMAT_RGB_565;
        }
        else if ((surface->redMaskSize == 5) && (surface->redPosition == 0)
    	&& (surface->greenMaskSize == 6) && (surface->greenPosition == 5)
    	&& (surface->blueMaskSize == 5) && (surface->bluePosition == 11))
        {
            //BBBBBGGGGGGRRRRR
            return VIDEO_BLIT_FORMAT_BGR_565;
        }
        else if ((surface->redMaskSize == 5) && (surface->redPosition == 10)
    	&& (surface->greenMaskSize == 5) && (surface->greenPosition == 5)
    	&& (surface->blueMaskSize == 5) && (surface->bluePosition == 0))
    	{
    	    //XRRRRRGGGGGBBBBB
    	    return VIDEO_BLIT_FORMAT_RGB_555;
    	}
    	else if ((surface->redMaskSize == 5) && (surface->redPosition == 0)
    	&& (surface->greenMaskSize == 5) && (surface->greenPosition == 5)
    	&& (surface->blueMaskSize == 5) && (surface->bluePosition == 10))
    	{
    	    //XBBBBBGGGGGRRRRR
    	    return VIDEO_BLIT_FORMAT_BGR_555;
    	}
    }
    
    //if no optimized format could be found, just use normal rgb(a)
    if (surface->bpp > 8)
    {
        if (surface->rsvMaskSize > 0)
  	        return VIDEO_BLIT_FORMAT_RGBA;
        else
            return VIDEO_BLIT_FORMAT_RGB;
    }
    
    //if all else fails, use the indexed mode
    //TODO: implement handling this mode
    return VIDEO_BLIT_FORMAT_INDEXCOLOR;
}

uint32_t vid_mapRGB(uint8_t red, uint8_t green, uint8_t blue, struct blit_surface* surface)
{
    //indexed?
    if (surface->modeType == VIDEO_MODE_TYPE_INDEXED)
    {
        //TODO: Handle palettes
        return 0;
    }
    else
    {
        return vid_mapRGBA(red, green, blue, 255, surface);
    }
}

uint32_t vid_mapRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, struct blit_surface* surface)
{
    //indexed?
    if (surface->modeType == VIDEO_MODE_TYPE_INDEXED)
    {
        return vid_mapRGB(red, green, blue, surface);
    }
    else
    {
        uint32_t value;

        red >>= (8 - surface->redMaskSize);
        green >>= (8 - surface->greenMaskSize);
        blue >>= (8 - surface->blueMaskSize);
        alpha >>= (8 - surface->rsvMaskSize);

        value = (red << surface->redPosition);
        value |= (green << surface->greenPosition);
        value |= (blue << surface->bluePosition);
        value |= (alpha << surface->rsvPosition);

        return value;
    }
}

void vid_unmapRGBA(uint32_t color, uint8_t* red, uint8_t* green, uint8_t* blue, uint8_t* alpha, struct blit_surface* surface)
{
    //indexed?
    if (surface->modeType == VIDEO_MODE_TYPE_INDEXED)
    {
        //TODO: Handle palettes
        *red = *green = *blue = *alpha = 0;
    }
    else if (surface->modeType == VIDEO_MODE_TYPE_RGB)
    {
        uint32_t tmp;

        //red
        tmp = color >> surface->redPosition;                //get all relevant bits
        tmp &= (1 << surface->redMaskSize) - 1;
        tmp <<= 8 - surface->redMaskSize;                   //expand value to 8 bits by shifting left
        uint32_t topBits = tmp >> (surface->redMaskSize);   //get the uppermost bits to fill the new lowermost
        tmp |= topBits;
        //tmp |= (1 << (8 - surface->redMaskSize)) - 1;     //this sets the least signifant bits that are now "new" to 1
        *red = tmp & 0xFF;

        //green
        tmp = color >> surface->greenPosition;
        tmp &= (1 << surface->greenMaskSize) - 1;
        topBits = tmp >> (surface->greenMaskSize - (8 - surface->greenMaskSize));
        tmp <<= 8 - surface->greenMaskSize;
        tmp |= topBits;
        //tmp |= (1 << (8 - surface->greenMaskSize)) - 1;
        *green = tmp & 0xFF;

        //blue
        tmp = color >> surface->bluePosition;
        tmp &= (1 << surface->blueMaskSize) - 1;
        topBits = tmp >> (surface->blueMaskSize - (8 - surface->blueMaskSize));
        tmp <<= 8 - surface->blueMaskSize;
        tmp |= topBits;
        //tmp |= (1 << (8 - surface->blueMaskSize)) - 1;
        *blue = tmp & 0xFF;

        //alpha
        if (surface->rsvMaskSize > 0)
        {
            tmp = color >> surface->rsvPosition;
            tmp &= (1 << surface->rsvMaskSize) - 1;
            topBits = tmp >> (surface->rsvMaskSize - (8 - surface->rsvMaskSize));
            tmp <<= 8 - surface->rsvMaskSize;
            tmp |= topBits;
            //tmp |= (1 << (8 - surface->rsvMaskSize)) - 1;
        }
        else //no alpha in this mode
            tmp = 255;

        *alpha = tmp & 0xFF;
    }
    else //unknown mode type...
    {
        print_string_static("WARNING: vid_unmapRGBA failed due to unknown mode type.\n");
        *red = *green = *blue = *alpha = 0;
    }
}

uint8_t* vid_getFramebufferPositionPointer(uint32_t x, uint32_t y, struct blit_surface* surface)
{
    uint8_t* ptr = surface->data;
    ptr += surface->bytesPerLine * y + x * surface->bytesPerPixel;
    
    return ptr;
}

uint32_t vid_getPixel(int x, int y, struct blit_surface* surface)
{
    switch(surface->bpp)
    {
    case 32:
        {
            uint32_t* ptr = (uint32_t*)vid_getFramebufferPositionPointer(x, y, surface);
            return *ptr;
        }
        
    case 24:
        {
            uint8_t* ptr = (uint8_t*)vid_getFramebufferPositionPointer(x, y, surface);
            return ptr[0] | ptr[1] << 8 | ptr[2] << 16;
        }
        break;
        
    case 16:
    case 15:
        {
            uint16_t* ptr = (uint16_t*)vid_getFramebufferPositionPointer(x, y, surface);
            return *ptr;
        }
        break;
        
    case 8:
        //TODO: palette...
        return 0;
        
    default:
        print_string_static("WARNING: vid_getPixel failed due to unknown bit depth.\n");
        return 0;
    }
}

void vid_setPixel(uint32_t color, int x, int y, struct blit_surface* surface)
{
    switch(surface->bpp)
    {
    case 32:
        {
            uint32_t* ptr = (uint32_t*)vid_getFramebufferPositionPointer(x, y, surface);
            *ptr = color;
        }
        break;
        
    case 24:
        {
            uint8_t* ptr = (uint8_t*)vid_getFramebufferPositionPointer(x, y, surface);
            uint8_t* colorPtr = (uint8_t*)&color;
            ptr[0] = colorPtr[0];
            ptr[1] = colorPtr[1];
            ptr[2] = colorPtr[2];
        }
        break;
        
    case 16:
    case 15:
        {
            uint16_t* ptr = (uint16_t*)vid_getFramebufferPositionPointer(x, y, surface);
            *ptr = (uint16_t)(color & 0xFFFF);
        }
        break;
        
    case 8:
        //TODO: palette...
        break;
        
    default:
        print_string_static("WARNING: vid_setPixel failed due to unknown bit depth.\n");
        break;
    }
}

void vid_fillRect(uint32_t color, int x, int y, int width, int height, struct blit_surface* surface)
{
    if(surface->bpp == 32){
        for(int yPos = 0; yPos < height; yPos++)
        {
            uint32_t* dstptr = (uint32_t*)vid_getFramebufferPositionPointer(x, y + yPos, surface);

            for(int xPos = 0; xPos < width; xPos++)
                *dstptr++ = color;
        }
    }
    else
    {
        for(int i = 0; i < width; i++)
            for(int j = 0; j < height; j++)
                vid_setPixel(color, x + i, y + j, surface);
    }
}

void vid_blit(int x, int y, int width, int height, int offset_x, int offset_y, struct blit_surface* src, struct blit_surface* dst)
{
    //FIXME: check bounds for coordinates!
    
    //check if we can do a direct block copy
    if(vid_getFramebufferBlitFormat(src) == vid_getFramebufferBlitFormat(dst))
    {
        //print_string_static("vid_blit: Using a block copy...\n");
        blit_blockcopy(x, y, width, height, offset_x, offset_y, src, dst);
    }
    //or use specific blitters
    /*else if(vid_getFramebufferBlitFormat(src) == VIDEO_BLIT_FORMAT_RGBA_8888 &&
            vid_getFramebufferBlitFormat(dst) == VIDEO_BLIT_FORMAT_BGRA_8888)
    {
        print_string_static("vid_blit: Using blit_RGBX8888toBGRX8888...\n");
        blit_RGBX8888toBGRX8888(x, y, width, height, offset_x, offset_y, src, dst);
    }
    else if(vid_getFramebufferBlitFormat(src) == VIDEO_BLIT_FORMAT_RGBA_8888 &&
            vid_getFramebufferBlitFormat(dst) == VIDEO_BLIT_FORMAT_BGR_888)
    {
        print_string_static("vid_blit: Using blit_RGBX8888toBGR888...\n");
        blit_RGBX8888toBGR888(x, y, width, height, offset_x, offset_y, src, dst);
    }
    else if(vid_getFramebufferBlitFormat(src) == VIDEO_BLIT_FORMAT_RGBA_8888 &&
            vid_getFramebufferBlitFormat(dst) == VIDEO_BLIT_FORMAT_RGB_888)
    {
        print_string_static("vid_blit: Using blit_RGBX8888toRGB888...\n");
        blit_RGBX8888toRGB888(x, y, width, height, offset_x, offset_y, src, dst);
    }
    else if(vid_getFramebufferBlitFormat(src) == VIDEO_BLIT_FORMAT_RGBA_8888 &&
            vid_getFramebufferBlitFormat(dst) == VIDEO_BLIT_FORMAT_ARGB_8888)
    {
        print_string_static("vid_blit: Using blit_RGBX8888toXRGB8888...\n");
        blit_RGBX8888toXRGB8888(x, y, width, height, offset_x, offset_y, src, dst);
    }
    else if(vid_getFramebufferBlitFormat(src) == VIDEO_BLIT_FORMAT_RGBA_8888 &&
            vid_getFramebufferBlitFormat(dst) == VIDEO_BLIT_FORMAT_ABGR_8888)
    {
        print_string_static("vid_blit: Using blit_RGBX8888toXGBR8888...\n");
        blit_RGBX8888toXBGR8888(x, y, width, height, offset_x, offset_y, src, dst);
    }
    
    else if(vid_getFramebufferBlitFormat(src) == VIDEO_BLIT_FORMAT_RGB_888 &&
            vid_getFramebufferBlitFormat(dst) == VIDEO_BLIT_FORMAT_BGRA_8888)
    {
        print_string_static("vid_blit: Using blit_RGB888toBGRX8888...\n");
        blit_RGB888toBGRX8888(x, y, width, height, offset_x, offset_y, src, dst);
    }
    else if(vid_getFramebufferBlitFormat(src) == VIDEO_BLIT_FORMAT_RGB_888 &&
            vid_getFramebufferBlitFormat(dst) == VIDEO_BLIT_FORMAT_RGBA_8888)
    {
        print_string_static("vid_blit: Using blit_RGB888toRGBX8888...\n");
        blit_RGB888toRGBX8888(x, y, width, height, offset_x, offset_y, src, dst);
    }
    else if(vid_getFramebufferBlitFormat(src) == VIDEO_BLIT_FORMAT_RGB_888 &&
            vid_getFramebufferBlitFormat(dst) == VIDEO_BLIT_FORMAT_BGR_888)
    {
        print_string_static("vid_blit: Using blit_RGB888toBGR888...\n");
        blit_RGB888toBGR888(x, y, width, height, offset_x, offset_y, src, dst);
    }*/
    else //or if we need to do it the slow way
    {
        //print_string_static("vid_blit: Using generic blitter...\n");
        blit_generic(x, y, width, height, offset_x, offset_y, src, dst);
    }
}
