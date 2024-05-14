#include <stdint.h>

void mem_copy(uint8_t *source, uint8_t *dest, uint16_t bytes){
	for(uint16_t i=0;i<bytes;i++)
		dest[i]=source[i];
}

uint8_t inbyte(uint16_t port){
	uint8_t result;
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

void outbyte(uint16_t port,uint8_t data){
	__asm__("out %%al, %%dx" : : "a" (data) , "d" (port));
}

//TODO Add word versions

