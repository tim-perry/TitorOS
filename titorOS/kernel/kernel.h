#ifndef KERNEL_H
#define KERNEL_H
#include <stdint.h>

struct boot_info_t {
    uint32_t memtable;
    uint32_t entries;
    uint32_t kaddr;
    uint32_t ksize;
};


extern void load_gdt();

#endif