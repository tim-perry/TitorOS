#include <stdint.h>
#include <drivers/utils.h>

void init_timer(uint16_t counter) {

	uint8_t hi = (uint8_t)((counter >> 8) & 0xFF);
	uint8_t lo = (uint8_t)(counter & 0xFF);

	outbyte(0x43, 0b00110110);
	outbyte(0x40, lo);
	outbyte(0x40, hi);
	return;
}




uint32_t time;

//called by timer interrupt
void tick() {
    time++;
}

void delay(uint32_t ms) {
    uint32_t time_at_call = time;
    while (time < time_at_call + ms);
}