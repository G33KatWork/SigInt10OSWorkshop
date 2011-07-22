#include <vgamode3.h>

static unsigned short *videoram = (unsigned short *) 0xb8000;
static unsigned int cursorX = 0;
static unsigned int cursorY = 0;

print_handler print_vgamode3_handler = {
    .init = vgamode3_init,
    .clear = vgamode3_clear,
    .putChar = vgamode3_putChar
};

static void scroll(void);

void vgamode3_init(void)
{
    vgamode3_clear();
}

void vgamode3_clear(void)
{
    cursorX = 0;
    cursorY = 0;
    
    unsigned char attributeByte = (backgroundColor << 4) | (foregroundColor & 0x0F);
    unsigned short blank = 0x20 /* space */ | (attributeByte << 8);
    
    for(int i = 0; i < 25*80; i++)
        videoram[i] = blank;
}

void vgamode3_putChar(char c)
{
    unsigned char attributeByte = (backgroundColor << 4) | (foregroundColor & 0x0F);
    unsigned short attribute = attributeByte << 8;
    unsigned short *location;
    
    //Backspace
    if(c == 0x08 && cursorX)
        cursorX--;
        
    //Tab
    else if(c == 0x09)
        cursorX = (cursorX+8) & ~(8-1);
    
    //Carriage Return
    else if(c == '\r')
        cursorX = 0;
    
    //New Line
    else if(c == '\n')
    {
        cursorX = 0;
        cursorY++;
    }
    
    //Normal character
    else if(c >= ' ')
    {
        location = videoram + (cursorY*80 + cursorX);
        *location = c | attribute;
        cursorX++;
    }
    
    //Handle end of line
    if (cursorX >= 80)
    {
        cursorX = 0;
        cursorY ++;
    }
    
    scroll();
}

void scroll(void)
{
    unsigned char attributeByte = (backgroundColor << 4) | (foregroundColor & 0x0F);
    unsigned short blank = 0x20 /* space */ | (attributeByte << 8);
    
    if(cursorY >= 25)
    {
        for(int i = 0; i < 24*80; i++)
            videoram[i] = videoram[i+80];
        
        //move all lines one line up and discard upper line
        //memmove(vidmem, vidmem + 80, 24*80);
        
        //clear lowest line
        for (int i = 24*80; i < 25*80; i++)
            videoram[i] = blank;
        
        cursorY = 24;
    }
}
