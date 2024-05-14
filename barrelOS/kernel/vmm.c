#include <stdint.h>
#include <kernel/vmm.h>
#include <kernel/pmm.h>
#include <drivers/utils.h>
#include <drivers/screen.h>
#include <drivers/utils.h>
#include <kernel/idt/interrupt_handlers.h>

extern uint32_t active_pid;

#define PDT_LOCATION 0xc0300000
#define PAGE_SIZE 4096

#define HEAP_BASE (void*)0xc0400000
#define HEAP_LIMIT (void*)0xc0800000

#define USER_HEAP_BASE (void*)0x10000000
#define USER_LIMIT (void*)0xc0000000

#define NULL 0

uint32_t* pdt;
uint8_t page_fault_allowed;

void init_vmm() {
    pdt = (uint32_t*)PDT_LOCATION;

    //map last entry of pdt to itself
    pdt[1023] = 0x0300000 + 0b100000000011;

    //free virtual address that holds pdt?

    //clear identity-mapped bootloader
    pdt[0] = 0;

    page_fault_allowed = 0;
}

void* kmalloc() {
    uint32_t* entry;
    void* vaddr;
    
    for (vaddr = HEAP_BASE; vaddr < HEAP_LIMIT; vaddr += 0x1000) {
        entry = page_entry(vaddr);
        if ((*entry & 0x800) == 0) goto found;
    }
    
    //kernel heap out of memory
    kpanic("Kernel heap out of memory");
    
    found:
    //if present, only set used bit
    //else set paddr and set flags
    if ((*entry & 0x00000001) == 1) {
        *entry |= 0x800;
    } else {
        *entry = (uint32_t)pmalloc() + 0b100000000011;
    }

    return vaddr;
}


void kfree(void* vaddr) {
    uint32_t* entry = page_entry(vaddr);
    *entry &= 0xFFFFF7FF; //turn off used bit
    asm volatile("invlpg (%0)": : "r"(vaddr): "memory");
}


void* mmiomalloc(void* paddr) {
    void* vaddr = kmalloc();
    uint32_t* entry = page_entry(vaddr);
    void* oldpaddr = (void*)(*entry & 0xFFFFF000);
    pfree(oldpaddr);

    *entry = (uint32_t)paddr + (*entry & 0x00000FFF);
    return vaddr;
}




//ERROR CODE
//bit 0 present (When set, the page fault was caused by a page-protection violation. When not set, it was caused by a non-present page)
//bit 1 write (When set, the page fault was caused by a write access. When not set, it was caused by a read access)
//bit 2 user (When set, the page fault was caused while CPL = 3. This does not necessarily mean that the page fault was a privilege violation)
//PTE FLAGS
//bit 0 present
//bit 1 write
//bit 2 user
//bit 3 write-through
//bit 4 cache disable
//bit 5 accessed
//bit 6 dirty
//bit 7 page attribute table
//bit 8 global
//bit 9-11 available
void page_fault(isr_stack_t stack) {
    //println("page fault");
    //get virtual address from cr2 register
	    void* vaddr;
	    __asm__("mov %%cr2, %%eax" : "=a" (vaddr) :);

    //caused by non-present page
    if ((stack.error & 0b1) == 0) {
        if (page_fault_allowed == 0) {
            println("while in pid ");
            print(hexstring(active_pid, 32));
            kpanic("nonpresent");
        }
        uint32_t* entry = page_entry(vaddr);
        *entry = (uint32_t)pmalloc() + 0b100000000111;
        mem_set((uint8_t*)((uint32_t)vaddr & 0xFFFFF000), 0, 0x1000);
    }

    //caused by privilege violation
    else if ((stack.error & 0b100) == 0b100 && (vaddr >= USER_LIMIT)) {
        kpanic("Tried to access kernel page while in CPL 3");
    }

    //copy on write
    else if ((stack.error & 0b110) == 0b110 && (vaddr < USER_LIMIT)) {
        //println("COPY ON WRITE");

        //setup relevent pointers
        void* page = (void*)((uint32_t)vaddr & 0xFFFFF000);
        uint32_t* entry = page_entry(page);
        void* page_table = (void*)((uint32_t)entry & 0xFFFFF000);
        uint32_t* dir_entry = page_entry(page_table);

        //if page directory entry write bit is not set
        if ((*dir_entry & 0b10) == 0) {
            //create copy of page table with write bits set to zero
            uint32_t* new_pt = kmalloc();
            mem_copy(page_table, new_pt, 4096);            
            for (uint16_t i = 0; i < 1024; i++) {
                new_pt[i] &= 0xFFFFFFFD; //turn off write bit
            }
            uint32_t table_frame = (uint32_t)getpaddr(new_pt);
            *page_entry(new_pt) = 0; //manually clear from heap
            asm volatile("invlpg (%0)": : "r"(new_pt):);

            //println(hexstring(*dir_entry, 32));
            //println(hexstring(*entry, 32));

            //point directory entry to new page table and set write bit
            *dir_entry &= 0x00000FFF;
            *dir_entry |= table_frame | 0b10;
            asm volatile("invlpg (%0)": : "r"(page_table):);

            //println(hexstring(*dir_entry, 32));
            //println(hexstring(*entry, 32));
            //while(1);
        }


        //create copy of page
        void* newpage = kmalloc();
        mem_copy(page, newpage, 4096);
        uint32_t page_frame = (uint32_t)getpaddr(newpage);
        *page_entry(newpage) = 0; //manually clear the page vaddr from the heap
        asm volatile("invlpg (%0)": : "r"(newpage):);

        //point page table entry to new page and set write bit
        *entry &= 0x00000FFF;
        *entry |= page_frame | 0b10;
        asm volatile("invlpg (%0)": : "r"(page):);
    }
    else panic(stack);
}

////asm volatile("movl %%cr3, %%eax\n\t movl %%eax, %%cr3" : : : "eax", "memory");
//asm volatile("invlpg (%0)": : "r"(page_table): "memory");
//asm volatile("invlpg (%0)": : "r"(page): "memory");




//if hint is zero, mmap anywhere really
//flags 1 means take hint literally even if NULL
void* mmap(void* hint, uint32_t length, uint8_t flags) {
    void* base;
    if (flags == 0) base = USER_HEAP_BASE;
    else base = hint;

    uint32_t no_pages = length / PAGE_SIZE;
    if (length % PAGE_SIZE != 0) no_pages++; //round up to page

    page_fault_allowed = 1;
    for (uint32_t i = 0; i < no_pages; i++) {
        *page_entry(base + 0x1000 * i) = (uint32_t)pmalloc() | 0b100000000111;
    }
    page_fault_allowed = 0;

    asm volatile("movl %%cr3, %%eax\n\t movl %%eax, %%cr3" : : : "eax", "memory");
    return base;

    /*

    uint32_t* entry;
    void* vaddr;
    page_fault_allowed = 1;
    
    for (vaddr = USER_HEAP_BASE; vaddr < HEAP_BASE; vaddr += 0x1000) {
        entry = page_entry(vaddr);
        if ((*entry & 0x800) == 0) goto found;
    }
    kpanic("A userspace heap out of memory");
    
    found:
    //if present, only set used bit
    //else set paddr and set flags
    if ((*entry & 0x00000001) == 1) {
        *entry = (*entry & 0xFFFFF000) + 0b100000000111;
    } else {
        *entry = (uint32_t)pmalloc() + 0b100000000111;
    }

    page_fault_allowed = 0;
    return vaddr;
    */
}





//returns a pointer to the page table entry that holds vaddr
uint32_t* page_entry(void* vaddr) {
    uint32_t entry = 1023u * 0x400000;
    entry += ((uint32_t)vaddr/0x400000) << 12;
    entry += (((uint32_t)vaddr & 0x003FF000)/0x1000) * 4;
    return (uint32_t*)entry;
}

void* getpaddr(void* vaddr) {
    uint32_t* entry = page_entry(vaddr);
    return (void*)(*entry & 0xFFFFF000);
}


void setallow(uint8_t allow) {
    page_fault_allowed = allow;
}

