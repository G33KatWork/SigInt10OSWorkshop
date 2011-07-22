#include <serial.h>
#include <ports.h>

static int portNum = SERIAL_COM1;

print_handler print_serial_handler = {
    .init = serial_init,
    .clear = serial_clear,
    .putChar = serial_putChar
};

static int serial_readyToTransmit(void);

void serial_init(void)
{
    portNum = SERIAL_COM1;
    
    outb(portNum + 1, 0x00);    // Disable all interrupts
    outb(portNum + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(portNum + 0, 0x01);    // Set divisor to 1 (lo byte) 115200 baud
    outb(portNum + 1, 0x00);    //                  (hi byte)
    outb(portNum + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(portNum + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(portNum + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(portNum + 1, 0x0C);    // enable all interrupts.
}

void serial_clear()
{   
}

void serial_putChar(char c)
{
	while(!serial_readyToTransmit());
        outb(portNum, c);
}

int serial_readyToTransmit()
{
    uint8_t status = inb(portNum + 5);
        return (status & 0x20);
}
