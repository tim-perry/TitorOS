#include <kernel/kernel.h>
#include <kernel/pmm.h>
#include <kernel/screen.h>

void kernel(struct boot_info_t* boot_table) {
    load_gdt();
    
    clearscreen();
    setcolor(0x60);
    print("Boot Table\n");
    print("\nMemory map location: ");
    printhex(boot_table->memtable);
    print("\nMemory map entries: ");
    printhex(boot_table->entries);
    print("\nKernel address: ");
    printhex(boot_table->kaddr);
    print("\nKernel size: ");
    printhex(boot_table->ksize);
    print("\n\n");

    //kernel stuff
    init_pmm(boot_table->memtable, boot_table->entries);

}

