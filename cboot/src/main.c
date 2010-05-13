#include <main.h>

int kmain(void* UNUSED(mbd), unsigned int magic)
{
    if ( magic != 0x2BADB002 )
    {
        /* Something went not according to specs. Print an error */
        /* message and halt, but do *not* rely on the multiboot */
        /* data structure. */
    }

    /* You could either use multiboot.h */
    /* (http://www.gnu.org/software/grub/manual/multiboot/multiboot.html#multiboot_002eh) */
    /* or do your offsets yourself. The following is merely an example. */ 
    //char * boot_loader_name = (char *)mbd + 64;

    /* Print a letter to screen to see everything is working: */
    unsigned char *videoram = (unsigned char *) 0xb8000;
    
    /* Clear screen */
    for(int i = 0; i < 25*80*2; i++)
        videoram[i] = 0;

    videoram[0] = 65; /* character 'A' */
    videoram[1] = 0x07; /* forground, background color. */

    /* <-- Insert kernel here --> */

    return 0;
}
