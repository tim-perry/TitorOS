#ifndef SCREEN_H
#define SCREEN_H
#include <stdint.h>

#define WIDTH 80
#define HEIGHT 25

typedef struct  __attribute__((packed)) pixel {
    char character;
    uint8_t color;
} pixel;

void setcursor(uint16_t position);
uint16_t getcursor();

void setcolor(uint8_t color);
void clearscreen();
void print(char* string);
void printhex(uint64_t hex);




#endif