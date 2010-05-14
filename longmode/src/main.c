#include <main.h>

int main()
{
    write_string(0x4F, "Welcome to the marvellous world of 64 Bits.");
    return 0;
}

void write_string(char colour, const char *string)
{
	volatile char *video=(volatile char*)0xB8000;
	while(*string!=0)
	{
		*video=*string;
		string++;
		video++;
		*video=colour;
		video++;
	}
}
