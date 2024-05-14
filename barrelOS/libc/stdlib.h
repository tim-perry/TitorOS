#include <stdint.h>

typedef struct memory_block {
    uint32_t size; //excluding this descriptor (bit 0 indicates used)
    void* fd;
    void* bk;
}__attribute__((packed)) mem_block_t;

void exit(uint32_t status);
void* malloc(uint32_t size);
void free(void* ptr);