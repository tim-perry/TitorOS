#include <kernel/screen.h>
#include <kernel/utils.h>

struct pixel* vidmem = (void*)0xb8000;

void setcursor(uint16_t position) {
    uint8_t hi = (position >> 8) & 0xFF;
	uint8_t lo = position & 0xFF;
	outbyte(0x3d4, 14);
	outbyte(0x3d5, hi);
	outbyte(0x3d4, 15);
	outbyte(0x3d5, lo);
}

uint16_t getcursor() {
	uint8_t hi;
	uint8_t lo;
	outbyte(0x3d4, 14);
	hi = inbyte(0x3d5);
	outbyte(0x3d4, 15);
	lo = inbyte(0x3d5);
	uint16_t position;
	position = (hi << 8) + lo;
	return position;
}


void setcolor(uint8_t color) {
    pixel* pixel = vidmem;
    for (uint32_t i = 0; i < WIDTH * HEIGHT; i++) {
        pixel[i].color = color;
    }
}

void clearscreen() {
    pixel* pixel = vidmem;
    for (uint32_t i = 0; i < WIDTH * HEIGHT; i++) {
        pixel[i].character = ' ';
    }
    setcursor(0);
}

void print(char* string) {
    pixel* pixel = vidmem;
    for (uint32_t i = 0; string[i] != 0; i++) {
        if (string[i] == '\n') {
            setcursor(getcursor() + (WIDTH - getcursor() % WIDTH));
        } else {
            pixel[getcursor()].character = string[i];
            setcursor(getcursor() + 1);
        }
        //check for scrolling
    }
}

void printhex(uint64_t hex) {
    print("0x");
    char hexstring[2] = {'X', 0};

    for (uint32_t i = 0; i < 16; i++) {
        uint8_t c = (hex >> (4 * (15 - i))) & 0x0f;
        if (c < 0xa) {
            c += 48; //to number ascii
        } else {
            c += 55; //to letter conversion
        }
        hexstring[0] = c;
        print(hexstring);
    }
}
