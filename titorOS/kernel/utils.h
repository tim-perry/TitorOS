#include <stdint.h>

void mem_set(void *addr, uint8_t value, uint32_t count);

uint8_t inbyte(uint16_t port);
void outbyte(uint16_t port,uint8_t data);
uint16_t inword(uint16_t port);
void outword(uint16_t port, uint16_t data);
uint32_t indword(uint16_t port);
void outdword(uint16_t port, uint32_t data);