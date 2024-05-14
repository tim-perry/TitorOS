#include <kernel/pmm.h>
#include <kernel/screen.h>

#define MIN_BLOCK 0x1000    //page frame
#define MAX_BLOCK 0x100000  //1 Mib

//TODO
//Buddy allocator
//pmalloc(number of contiguous) (OOM detection)
//pfree

//globals
uint64_t total;
uint64_t free;
uint64_t** bitmap;
uint64_t regions;

void init_pmm(uint32_t memtable_addr, uint32_t entries) {
    total = 0;
    free = 0;

    //parse memtable
    regions = entries;
    memtable* table = (memtable*)(uint64_t)memtable_addr;
    for(int i = 0; i < entries; i++) {
        //add to total memory count
        total += table[i].size;

        //list entries
        print("entry:\n");
        printhex(table[i].address);
        print("-");
        printhex(table[i].size);
        print("-");
        printhex(table[i].type);
        print("-");
        printhex(table[i].extended);
        print("\n");
    }
    print("total: ");
    printhex(total);
    
    uint64_t bmap_size = total / 0x1000; //number of pages (bits)
   




    //initialize page frame usage
        //wherever kernel is (map specific physical pages similar to some kind of ioalloc?)
        //pmm data structure itself
}

/*
void* pmalloc() {
    //index the bit of the next free page frame
    uint64_t i = 0;
    uint8_t j = 0;

    //for (; i < SIZE; i++) {
    //    if (pf_bitmap[i] != 0xff) break;
    //}

    for (; j < 32; i++) {
        if (((pf_bitmap[i] >> j) & 0x1) == 0) break; //page frame is free
    }

    pf_bitmap[i] &= (0x1 << j); //set page frame as used
    return (void*)((i * 32 + j) * 0x1000); //return page frame address
}
*/
/*
void pfree(void* addr) {
    //return number of pages freed?
}
*/