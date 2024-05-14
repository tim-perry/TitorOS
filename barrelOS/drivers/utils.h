#include <stdint.h>
char* hexstring(uint32_t value, uint8_t width);
void mem_set(void* addr, uint8_t value, uint32_t count);
void mem_copy(void* source, void* dest, uint16_t bytes);
uint8_t inbyte(uint16_t port);
void outbyte(uint16_t port,uint8_t data);
uint16_t inword(uint16_t port);
void outword(uint16_t port, uint16_t data);
uint32_t indword(uint16_t port);
void outdword(uint16_t port, uint32_t data);