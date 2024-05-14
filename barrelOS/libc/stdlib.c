#include <stdint.h>
#include <libc/syscalls.h>
#include <libc/stdlib.h>

#define PAGE_SIZE 4096
#define NULL 0
#define USED 1

extern void* heap;

void exit(uint32_t status) {
    //call atexit functions
    //close file descriptors?
    _exit(status);
}

void* malloc(uint32_t size) {
    mem_block_t* block = heap;
    size += ((size % 4 == 0) ? 0 : 4 - (size % 4)); //round up to nearest 4 bytes

    //search for unused block of sufficient size
    while((block->size & 0b1) == USED || (block->size < size)) {
        block = block->fd;
        if (block == NULL) {
            //get more memory
            //void* base = mmap(NULL, PAGE_SIZE * PAGE_SIZE);
            while(1);
        }
    }

    //if required block size is less than half the size of the block (arbitrary), split it
    if (size < block->size / 2) {
        mem_block_t* split = (void*)block + sizeof(mem_block_t) + size; //create new block descriptor at appropriate address (should be 4byte aligned)
        split->size = block->size - size - sizeof(mem_block_t);
        split->size &= 0xFFFFFFFC; //ensure marked free, and 4byte aligned
        
        block->size = size;
        //link into list
        split->bk = block;
        split->fd = block->fd;
        block->fd = split;
    }
    block->size |= 1; //mark as used
    return (void*)block + sizeof(mem_block_t);
}

void free(void* ptr) {
    mem_block_t* block = ptr - sizeof(mem_block_t);
    block->size &= 0xFFFFFFFE; //mark unused
}

void* realloc(void* ptr, uint32_t size) {

}