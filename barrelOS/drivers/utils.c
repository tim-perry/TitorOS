#include <stdint.h>
#include <kernel/vmm.h>
#include <drivers/utils.h>


char* hexstring(uint32_t value, uint8_t width) {
	
	char* string = "0x";
	uint8_t digit;
	int i = 0;
	for (; i < width/4; i++) {
		digit = (value >> ((width - 4) - i*4)) & 0xF;
		digit += 48;
		if (digit > 57) digit+= 7;
		string[i+2] = (char)digit;
	}
	string[i+2] = 0;
	return string;
}

void mem_set(void* addr, uint8_t value, uint32_t count) {
	uint8_t* byte = addr;
	for (uint32_t i = 0; i < count; i++) {
		byte[i] = value;
	}
}


void mem_copy(void* source_addr, void* dest_addr, uint16_t bytes) {
	uint8_t* source = source_addr;
	uint8_t* dest = dest_addr;

	if (bytes > 4096) kpanic("mem_copy exceeds size of page");
	uint8_t* buffer = kmalloc();
	for(uint16_t i = 0; i < bytes; i++) {
		buffer[i]=source[i];
	}
	for(uint16_t i = 0; i < bytes; i++) {
		dest[i]=buffer[i];
	}
	kfree(buffer);
}

uint8_t inbyte(uint16_t port) {
	uint8_t result;
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

void outbyte(uint16_t port,uint8_t data) {
	__asm__("out %%al, %%dx" : : "a" (data) , "d" (port));
}

uint16_t inword(uint16_t port) {
	uint16_t result;
	__asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}

void outword(uint16_t port, uint16_t data) {
	__asm__("out %%ax, %%dx" : : "a" (data) , "d" (port));
}

uint32_t indword(uint16_t port) {
	uint32_t result;
	__asm__("inl %%edx, %%eax" : "=a" (result) : "d" (port));
	return result;
}

void outdword(uint16_t port, uint32_t data) {
	__asm__("outl %%eax, %%edx" : : "a" (data) , "d" (port));
}