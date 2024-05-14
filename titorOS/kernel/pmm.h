#ifndef PMM_H
#define PMM_H
#include <stdint.h>

void init_pmm();
void* pmalloc();
void pfree(void* addr);

typedef struct memtable {
    uint64_t address;
    uint64_t size;
    uint32_t type;
    uint32_t extended;
} memtable;

typedef struct region {
    uint64_t size;
    uint64_t** bitmap;

} region;

#endif