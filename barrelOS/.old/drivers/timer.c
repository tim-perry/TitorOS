#include <stdint.h>
#include <drivers/screen.h>

extern void tick(){
	uint8_t *time = 0x0;
	putchar(*time,0,0,0x41);
	*time+=1;
}
