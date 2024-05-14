#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
void clearscreen();
void setcolor(uint8_t);
void setpixel(uint16_t, char, uint8_t);
void setcursor(uint16_t);
uint16_t getcursor();
void print(char*);
void println(char*);
void scroll();

void init_terminal();
void keybuffer(uint8_t key);
void complete_buffer();
uint8_t sendbuffer(void* destination, uint8_t max_length);

#endif