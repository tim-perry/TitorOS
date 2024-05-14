#include <kernel/utils.h>

void mem_set(void *addr, uint8_t value, uint32_t count) {
    uint8_t* byte = addr;
    for (uint32_t i = 0; i < count; i++) {
        byte[i] = value;
    }
}

//void mem_cpy(void* src, void* dst, uint32_t count) {
    //determine copy order by looking at dst > src or otherway
//}


uint8_t inbyte(uint16_t port) {
	uint8_t result;
	asm("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

void outbyte(uint16_t port,uint8_t data) {
	asm("out %%al, %%dx" : : "a" (data) , "d" (port));
}

uint16_t inword(uint16_t port) {
	uint16_t result;
	asm("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}

void outword(uint16_t port, uint16_t data) {
	asm("out %%ax, %%dx" : : "a" (data) , "d" (port));
}

uint32_t indword(uint16_t port) {
	uint32_t result;
	asm("inl %%edx, %%eax" : "=a" (result) : "d" (port));
	return result;
}

void outdword(uint16_t port, uint32_t data) {
	asm("outl %%eax, %%edx" : : "a" (data) , "d" (port));
}

//inqword
//outqword