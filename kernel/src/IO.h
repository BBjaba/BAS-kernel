#pragma once
#include <stdint.h>

void outb (uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
void io_wait();
bool getBit(uint8_t byte, int position);
void outw(unsigned short port, unsigned short value);
