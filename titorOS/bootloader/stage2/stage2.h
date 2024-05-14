#ifndef STAGE2_H
#define STAGE2_H

#include <stdint.h>
#define PAGE_SIZE 0x1000
#define SECTOR_SIZE 512

//memory region types
#define USABLE 1
#define UNUSABLE 2
#define ACPI_RECLAIMABLE 3
#define ACPI_NVS 4
#define BAD 5

typedef struct sector {
    unsigned char byte[SECTOR_SIZE];
} sector;

typedef struct page {
    uint64_t entry[512];
} page;

struct boot_info {
    void* table;
    uint32_t entries;
    void* kaddr;
    uint32_t ksize;
};

typedef struct memtable {
    uint64_t address;
    uint64_t size;
    uint32_t type;
    uint32_t extended;
} memtable;

//functions
void load_gdt();
void mmap();
void init_kernel(void*);
void mem_set(void* dst, uint8_t value, uint32_t count);
void mem_cpy(void* src, void* dst, uint32_t count);
void bios_read(uint32_t drive, uint32_t sector, void* dest, uint32_t count);
uint32_t bios_e820(void* dest);
void bios_print(char* string);

#endif