#include <bitmap.h>
#include <print.h>
#include <lib.h>

static void bitmap_mask_to_pos_and_len(uint32_t mask, uint8_t* pos, uint8_t* len)
{
    uint8_t length = 0;
    uint8_t position = 0;
    
    if(mask != 0)
    {
        while(!(mask & 1))
        {
            mask >>= 1;
            position++;
        }

        mask++;
        while(!(mask & 1))
        {
            mask >>= 1;
            length++;
        }
    }
    
    *len = length;
    *pos = position;
}

int bitmap_check_support(const uint8_t* bitmapdata)
{
	const bitmap_file_hdr* fileheader = bitmap_get_file_header(bitmapdata);
	
	//check magic value
	if(!(fileheader->magic[0] == 'B' && fileheader->magic[1] == 'M'))
		return 0;
	
	const bitmap_info_hdr* infoheader = bitmap_get_info_header(bitmapdata);
	
	//no compression or bitfield stuff!
	if(infoheader->compression_type != BITMAP_COMP_RGB && infoheader->compression_type != BITMAP_COMP_BITFIELDS)
	{
        print_string_static("Bitmap has compression: ");
        print_integer(infoheader->compression_type, 10);
        print_string_static(" - Do not like!\n");
		return 0;
	}
	
	//normal info header is 40 byte long
	if(infoheader->headersize != 40 && infoheader->headersize != 56)
	{
        print_string_static("Bitmap wrong infoheader size: ");
        print_integer(infoheader->headersize, 10);
        print_string_static(" - Do not like!\n");
		return 0;
	}
	
	//only 8, 16, 24 and 32 bits per pixel
	//TODO: palletized 8bpp
	if(/*infoheader->bitspp != 8 &&*/ infoheader->bitspp != 16 && infoheader->bitspp != 24 && infoheader->bitspp != 32)
		return 0;
		
	return 1;
}

void bitmap_get_blit_surface(const uint8_t* bitmapdata, struct blit_surface* surface)
{
	const bitmap_info_hdr* infoheader = bitmap_get_info_header(bitmapdata);
	
	//TODO: negative height for upside-down bitmaps
	surface->width = abs(infoheader->width);
	surface->height = abs(infoheader->height);
	
	surface->data = bitmapdata + bitmap_get_file_header(bitmapdata)->bmp_offset;
	
	print_string_static("Bitmap: compression type is ");
    print_integer(infoheader->compression_type, 10);
    print_string_static("\n");
	
    surface->page = 0;
	
	if(infoheader->compression_type == BITMAP_COMP_RGB)
	{
    	switch(infoheader->bitspp)
    	{
    	case 16:
            print_string_static("Bitmap: 15bpp\n");
        
            surface->rsvMaskSize = 1;
    		surface->rsvPosition = 15;
		
    		surface->redMaskSize = 5;
    		surface->redPosition = 10;	//shift left amount
		
    		surface->greenMaskSize = 5;
    		surface->greenPosition = 5;
		
    		surface->blueMaskSize = 5;
    		surface->bluePosition = 0;
		
            surface->bpp = 15;
            surface->bytesPerPixel = 2;
		
            surface->modeType = VIDEO_MODE_TYPE_RGB;
            surface->bytesPerLine = ((16*infoheader->width)/32)*4;
    		break;
		
    	case 24:
    	    print_string_static("Bitmap: 24bpp\n");
    		surface->redMaskSize = 8;
    		surface->redPosition = 16;

    		surface->greenMaskSize = 8;
    		surface->greenPosition = 8;

    		surface->blueMaskSize = 8;
    		surface->bluePosition = 0;

    		surface->rsvMaskSize = 0;
    		surface->rsvPosition = 0;
		
    		surface->bpp = 24;
            surface->bytesPerPixel = 3;
        
            surface->modeType = VIDEO_MODE_TYPE_RGB;
            surface->bytesPerLine = ((24*infoheader->width)/32)*4;
    		break;
		
    	case 32:
    	    print_string_static("Bitmap: 32bpp\n");
    		surface->redMaskSize = 8;
    		surface->redPosition = 16;

    		surface->greenMaskSize = 8;
    		surface->greenPosition = 8;

    		surface->blueMaskSize = 8;
    		surface->bluePosition = 0;

    		surface->rsvMaskSize = 8;
    		surface->rsvPosition = 24;
		
    		surface->bpp = 32;
            surface->bytesPerPixel = 4;
		
            surface->modeType = VIDEO_MODE_TYPE_RGB;
            surface->bytesPerLine = ((32*infoheader->width)/32)*4;
    		break;
    	}
    }
    else if(infoheader->compression_type == BITMAP_COMP_BITFIELDS)
    {
        print_string_static("Bitmap with bitfield compression: ");
        print_integer(infoheader->bitspp, 10);
        print_string_static(" bpp\n");
        
    	surface->bpp = infoheader->bitspp;
        surface->bytesPerPixel = infoheader->bitspp / 8;
        
        print_string_static("Bitmap: red mask ");
        print_integer(infoheader->redmask, 16);
        print_string_static("\n");
        
        print_string_static("Bitmap: green mask ");
        print_integer(infoheader->greenmask, 16);
        print_string_static("\n");
        
        print_string_static("Bitmap: blue mask ");
        print_integer(infoheader->bluemask, 16);
        print_string_static("\n");
        
        print_string_static("Bitmap: alpha mask ");
        print_integer(infoheader->alphamask, 16);
        print_string_static("\n");
        
        bitmap_mask_to_pos_and_len(infoheader->redmask, &surface->redPosition, &surface->redMaskSize);
        bitmap_mask_to_pos_and_len(infoheader->greenmask, &surface->greenPosition, &surface->greenMaskSize);
        bitmap_mask_to_pos_and_len(infoheader->bluemask, &surface->bluePosition, &surface->blueMaskSize);
        bitmap_mask_to_pos_and_len(infoheader->alphamask, &surface->rsvPosition, &surface->rsvMaskSize);
    
        surface->modeType = VIDEO_MODE_TYPE_RGB;
        surface->bytesPerLine = ((infoheader->bitspp*infoheader->width)/32)*4;
    }
    else
    {
        print_string_static("Unknown bitmap compression type\n");
    }
}
