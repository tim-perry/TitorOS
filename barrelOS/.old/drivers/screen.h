#include <stdint.h>

struct __attribute__((packed)) screenchar {
	uint8_t character;
	uint8_t colour;
};

void putchar(uint8_t character, int8_t xpos, int8_t ypos, uint8_t colour);
void wipe_screen(uint8_t character, uint8_t colour);
void scroll();

void set_cursor(uint16_t cursorpos);
uint16_t get_cursor();

