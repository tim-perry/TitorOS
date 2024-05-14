#include <bootloader/stage2/stage2.h>
#include <bootloader/stage2/elf.h>

#define MEMTABLE (memtable*)0x20000
#define PAGE_TABLES (page*)0x10000
#define FLAGS 3

struct boot_info boot;
memtable* table;

void stage2(uint32_t drive) {
    //load new gdt
    load_gdt();
    
    //Load first sector of elf
    sector elf_sector;
    uint32_t disk_sector_base = 1;
    Elf64_Ehdr* header = (void*)&elf_sector;
    for(;;disk_sector_base++) {
        bios_read(drive, disk_sector_base, &elf_sector, 1);
        if (header->e_ident[0] == 0x7f && header->e_ident[1] == 'E' && header->e_ident[2] == 'L' && header->e_ident[3] == 'F')
            break;
    }
    Elf64_Phdr* prog_header = (void*)&elf_sector + header->e_phoff;

    //generate memory map
    boot.ksize = prog_header->p_memsz;
    if (boot.ksize % PAGE_SIZE != 0) boot.ksize += PAGE_SIZE - boot.ksize % PAGE_SIZE; //round up to nearest page
    mmap();

    //load kernel segments
    sector seg_sector;
    uint32_t disk_sector = disk_sector_base + prog_header->p_offset / SECTOR_SIZE;
    uint32_t pos = 0;
    for (; pos <= prog_header->p_filesz; pos += SECTOR_SIZE) {
        bios_read(drive, disk_sector + pos / SECTOR_SIZE, &seg_sector, 1);
        mem_cpy(&seg_sector, boot.kaddr + pos, SECTOR_SIZE);
    }
    if (pos < prog_header->p_memsz)
        mem_set(boot.kaddr + pos, 0, prog_header->p_memsz - pos);

    //create page tables
    mem_set(PAGE_TABLES, 0x00, 7 * PAGE_SIZE);
    page* pml4 = PAGE_TABLES;

    page* identity_pdpt = PAGE_TABLES + 1;
    page* identity_pd = PAGE_TABLES + 2;
    page* identity_pt = PAGE_TABLES + 3;
    pml4->entry[0] |= (uint32_t)identity_pdpt + FLAGS;
    identity_pdpt->entry[0] |= (uint32_t)identity_pd + FLAGS;
    identity_pd->entry[0] |= (uint32_t)identity_pt + FLAGS;
    for (uint32_t i = 0; i < 512; i++)
        identity_pt->entry[i] = i * PAGE_SIZE + FLAGS;

    page* kernel_pdpt = PAGE_TABLES + 4;
    page* kernel_pd = PAGE_TABLES + 5;
    page* kernel_pt = PAGE_TABLES + 6;
    pml4->entry[511] |= (uint32_t)kernel_pdpt + FLAGS;
    kernel_pdpt->entry[510] |= (uint32_t)kernel_pd + FLAGS;
    kernel_pd->entry[0] |= (uint32_t)kernel_pt + FLAGS;
    for (uint32_t i = 0; i < 512; i++)
        kernel_pt->entry[i] = (uint32_t)boot.kaddr + i * PAGE_SIZE + FLAGS;


    init_kernel(PAGE_TABLES);
}

void mmap() {
    table = (void*)MEMTABLE;
    boot.table = MEMTABLE;
    uint32_t entries = bios_e820(table); //generate memory map with e820 bios function (real mode)

    //combine overlaps and ignore first 1mb
    for(uint32_t i = 0; i < entries; i++) {
        if (table[i].type != USABLE) continue;  //skip UNUSABLE entries
        uint64_t start = table[i].address;
        uint64_t end = table[i].address + table[i].size;

        //check for overlaps with other usable entries
        for (uint32_t k = i + 1; k < entries; k++) {
            if (table[k].type != USABLE) continue;
            uint64_t kstart = table[k].address;
            uint64_t kend = table[k].address + table[k].size;

            if (start <= kstart && kstart <= end) {
                if (kend > end) end = kend;
            } else if (start <= kend && kend <= end) {
                if (kstart < start) start = kstart;
            } else if (kstart <= start && kend >= end) {
                start = kstart;
                end = kend;
            } else {
                continue;
            }
            table[k].type = UNUSABLE; //mark merged entry as unusable
        }

        //cut off first 1mb
        if (end <= 0x100000) {
            table[i].type = UNUSABLE;
        }
        if (start < 0x100000) {
            start = 0x100000;
        }

        //modify entry
        table[i].address = start;
        table[i].size = end - start;
    }

    //align regions to page boundaries
    for(uint32_t i = 0; i < entries; i++) {
        uint64_t misalignment;

        misalignment = table[i].address % PAGE_SIZE;
        if (misalignment != 0) {
            table[i].address += PAGE_SIZE - misalignment;   //increase region address to start of next page
            table[i].size -= PAGE_SIZE - misalignment;      //decrease region size accordingly
        }
        misalignment = (table[i].address + table[i].size) % PAGE_SIZE;
        if (misalignment != 0) {
            table[i].size -= misalignment;                  //decrease region size to nearest page boundary
        }
    }

    //truncate list
    uint32_t j = 0; //number of usable entries
    for(uint32_t i = 0; i < entries; i++) {
        if (table[i].type != USABLE) continue;
        mem_cpy(table + i, table + j, sizeof(memtable));
        j++; //increment number of usable entries
    }
    boot.entries = j;

    //find suitable slot for kernel
    for (uint32_t i = 0; i < boot.entries; i++) {
        if (table[i].size >= boot.ksize) {
            boot.kaddr = (void*)(uint32_t)table[i].address;
            return;
        }
    }
    bios_print("Can't load kernel");
    while(1);
}

void mem_set(void* dst, uint8_t value, uint32_t count) {
    uint8_t* byte = dst;
    for (uint32_t i = 0; i < count; i++)
        byte[i] = value;
}

void mem_cpy(void* src, void* dst, uint32_t count) {
    uint8_t* src_byte = src;
    uint8_t* dst_byte = dst;
    for (uint32_t i = 0; i < count; i++)
        dst_byte[i] = src_byte[i];
}