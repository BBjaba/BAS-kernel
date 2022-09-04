#include "IO.h"

void outb(uint16_t port, uint8_t value){
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port){
    uint8_t returnVal;
    asm volatile ("inb %1, %0"
    : "=a"(returnVal)
    : "Nd"(port));
    return returnVal;
}

void outw(unsigned short port, unsigned short value)
{
    asm("outw %0, %1"
        :
        :"a"(value),
        "Nd"(port));
}

void io_wait(){
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

bool getBit(uint8_t byte, int position)
{
    return (byte >> position) & 0x1;
}

void sleep(uint32_t timer_count)
{
  while(1){
    asm volatile("nop");
    timer_count--;
    if(timer_count <= 0)
      break;
  }
}