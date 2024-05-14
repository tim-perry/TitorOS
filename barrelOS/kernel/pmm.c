#include <stdint.h>
#include <kernel/pmm.h>
#include <drivers/utils.h>
#include <drivers/screen.h>
#include <drivers/utils.h>
#include <kernel/idt/interrupt_handlers.h>

uint32_t* pf_bitmap;

void init_pf_bitmap() {
    pf_bitmap = (uint32_t*)0xc02e0000;
    mem_set((uint8_t*)pf_bitmap, 0, 0x20000);

    //first 4mb are used for kernel (up to 0x400000)
    //next 4mb are used for kernel stack (up to 0x800000)
    for (void* i = 0; i < (void*)0x800000; i+= 0x1000) {
        puse(i);
    }

    //reserve space for mmio
    //TODO reserve other space from mmap
}

void puse(void* paddr) {
    uint32_t page = (uint32_t)paddr/0x1000;
    uint8_t shift = page % 32;
    uint32_t index = page / 32;
    pf_bitmap[index] |= 0x1 << shift;
}

void pfree(void* paddr) {
    uint32_t page = (uint32_t)paddr/0x1000;
    uint8_t shift = page % 32;
    uint32_t index = page / 32;
    pf_bitmap[index] &= ~(0x1 << shift);
}

//currently very slow
void* pmalloc() {
    uint32_t i = 0;
    uint8_t shift = 0;

    while(i < 0x8000) {
        if (pf_bitmap[i] != 0xFFFFFFFF) goto index;
        i++;
    }
    clearscreen();
    kpanic("Out of memory!");

    index:
    while(!(((~pf_bitmap[i] >> shift) & 0x1) == 0x1)) {
        shift++;
    }

    uint32_t page = i * 32 + shift;
    void* paddr = (void*)(page * 0x1000);
    puse(paddr);
    return paddr;
}

//TODO
//add pmalloc for n contiguous pages (for DMA stuff)
//void* pmalloc_contig(uint8_t n) {}