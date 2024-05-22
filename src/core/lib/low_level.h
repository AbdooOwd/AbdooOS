#ifndef LOW_LEVEL_H
#define LOW_LEVEL_H

#include "../include/types.h"

unsigned char port_byte_in(unsigned short port);
void port_byte_out(unsigned short port, unsigned char data);
unsigned short port_word_in(unsigned short port);
void port_word_out(unsigned short port, unsigned short data);

static inline void outb(uint16_t port, uint8_t val);
static inline uint8_t inb(uint16_t port);

#endif