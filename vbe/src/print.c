#include <print.h>
#include <lib.h>

Color foregroundColor = White;
Color backgroundColor = Black;

print_handler* currentHandler = NULL;

static const char numChars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};


void print_init(print_handler* handler)
{
    currentHandler = handler;
    currentHandler->init();
}

static void reverseArray(char* arr)
{
    int i, j;
    for (i = 0, j = strlen(arr)-1; i<j; i++, j--)
    {
        int c = arr[i];
        arr[i] = arr[j];
        arr[j] = c;
    }
}

void print_clear(void)
{
    if(currentHandler)
        currentHandler->clear();
}

void print_string(char* s)
{
    while(*s)
    {
        print_char(*s);
        s++;
    }
}

void print_string_static(const char* s)
{
    print_string((char*)s);
}

void print_char(char c)
{
    if(currentHandler)
        currentHandler->putChar(c);
}

void print_integer(uint32_t n, uint8_t radix)
{
    char num[20];
    memset(num, 0, 20);
    
    int i = 0;
    do
    {
        num[i] = numChars[(n % radix)];
        n /= radix;
        
        i++;
    } while(n != 0);
    
    reverseArray(num);
    print_string(num);
}
