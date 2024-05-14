#include <libc/syscalls.h>
#include <libc/stdlib.h>

#define PAGE_SIZE 4096
#define HEAP_SIZE 40960
#define NULL 0
#define NOT_LITERAL 0

void* heap;

void init_libc() {
    //setup standard streams
    open("/dev/tty", 0x1); //stdin
    open("/dev/tty", 0x2); //stdout
    open("/dev/tty", 0x2); //stderr

    //setup heap for malloc
    heap = mmap(NULL, HEAP_SIZE, NOT_LITERAL);
    mem_block_t* block = heap;
    block->size = HEAP_SIZE;
    block->fd = NULL;
    block->bk = NULL;
}