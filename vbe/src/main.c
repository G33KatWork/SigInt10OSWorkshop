#include <gdt.h>
#include <idt.h>
#include <registers.h>
#include <print.h>
#include <global.h>
#include <pic.h>
#include <interrupts.h>
#include <pit.h>
#include <v86.h>
#include <lib.h>
#include <vbe.h>
#include <video.h>
#include <bitmap.h>
#include <serial.h>
#include <vgamode3.h>

#include <bitmap_file.h>

void cpu_init(void);
int kmain(void* mbd, unsigned int magic);
void timerHandler(registers_t* regs);

void timerHandler(registers_t* UNUSED(regs))
{
	print_string_static("TickTack\n");
}

void cpu_init()
{
    gdt_set_descriptor(0, 0, 0, 0, 0);
	gdt_set_descriptor (1,0,0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_EXEC_CODE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_PRESENT,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);
	gdt_set_descriptor (2,0,0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_PRESENT,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);
	
    gdt_install();
}

int kmain(void* UNUSED(mbd), unsigned int UNUSED(magic))
{
    //print_init(&print_vgamode3_handler);
    print_init(&print_serial_handler);
    print_string_static("v86 demo\n");
    
    cpu_init();					//set up GDT
    idt_install();				//set up IDT
	interrupts_init();			//set up callback table
	pic_init();					//set up PIC
    
    v86_init();
    print_string_static("v86 initialized\n");
    
    interrupts_enable();
	/*interrupts_registerHandler(IRQ_TIMER, timerHandler);*/
	pit_init(1000);
	//pic_unmask_irq(IRQ_TIMER);
    
    struct vbeControllerInfo* controllerInfo = vbe_getControllerInfo();
    print_string_static("VBE OEM vendor name: ");
    print_string((char*)REALMODE_PTR(controllerInfo->oemVendorNamePtr[1], controllerInfo->oemVendorNamePtr[0]));
    print_string_static("\n");
    print_string_static("VBE OEM product name: ");
    print_string((char*)REALMODE_PTR(controllerInfo->oemProductNamePtr[1], controllerInfo->oemProductNamePtr[0]));
    print_string_static("\n");
    
    /*uint16_t* modeArray = (uint16_t*)REALMODE_PTR(controllerInfo->videomodes[1], controllerInfo->videomodes[0]);
    
    while(*modeArray != 0xFFFF)
    {
        struct vbeModeInfo* modeInfo = vbe_getModeInfo(*modeArray);
        
        if(!modeInfo)
        {
            print_string_static("Modeinfo retrieval failed: ");
            print_integer(*modeArray, 16);
            continue;
        }
        
        print_integer(*modeArray, 16);
        
        if(modeInfo->attributes & VBEMODE_ATTR_HWSUPPORT)
            print_string_static(" Available Mode: ");
        else
            print_string_static("Unvailable Mode: ");
        
        if(modeInfo->attributes & VBEMODE_ATTR_COLOR)
            print_string_static("Color - ");
        else
            print_string_static("Monochrome - ");
        
        if(modeInfo->attributes & VBEMODE_ATTR_GRAPHICS)
            print_string_static("Graphics - ");
        else
            print_string_static("Text - ");
        
        if(modeInfo->attributes & VBEMODE_ATTR_LINEARFRAMEBUF)
            print_string_static("Linear FB - ");
        else
            print_string_static("Nonlinear FB - ");
            
        print_string_static(" Mem: ");
        print_integer(modeInfo->memoryModel, 16);
        print_string_static(" ");
        
        print_string_static(" Res: ");
        print_integer(modeInfo->Xres, 10);
        print_string_static("x");
        print_integer(modeInfo->Yres, 10);
        print_string_static("x");
        print_integer(modeInfo->bpp, 10);
        
        print_string_static("\n");
        
        modeArray++;
    }*/
    
    uint16_t closestMode = vbe_findClosestMode(800, 600, 32);
    vbe_setMode(closestMode);

    //get framebuffer surface
    struct blit_surface fbSurface;
    vid_getBlitSurfaceFromVBEMode(&vbe_currentVbeMode, &fbSurface);
    
    //get offscreen surface
    struct blit_surface offscreenSurface;
    vid_createOffscreenSurfaceFromVBEMode(&vbe_currentVbeMode, &offscreenSurface);
    
    //get bitmap surface
    if(!bitmap_check_support(bitmap_file))
		print_string_static("Bitmap doesn't seem to be in a supported format\n");
		    
    struct blit_surface bitmapSurface;
    bitmap_get_blit_surface(bitmap_file, &bitmapSurface);
    
    uint32_t bgcolor = vid_mapRGB(16, 67, 115, &fbSurface);
    uint32_t x = 0, y = 0;
	for(;;)
	{
	    //clear screen
        vid_fillRect(bgcolor, 0, 0, 800, 600, &offscreenSurface);
	    
	    //draw silly cat
	    vid_blit(x, y, bitmapSurface.width, bitmapSurface.height, 0, 0, &bitmapSurface, &offscreenSurface);
	    
        x+=5;
	    if(x > 800 - bitmapSurface.width)
            x = 0;
        
        //copy buffer from offscreen surface to framebuffer
        vid_blit(0, 0, 800, 600, 0, 0, &offscreenSurface, &fbSurface);
	}
    return 0;
}
