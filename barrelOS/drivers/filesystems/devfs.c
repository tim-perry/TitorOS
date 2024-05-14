#include <drivers/filesystems/devfs.h>
#include  <stdint.h>
#include <kernel/vfs.h>
#include <drivers/utils.h>
#include <kernel/idt/interrupt_handlers.h>
#include <drivers/nvme.h>
#include <drivers/pci.h>
#include <kernel/vmm.h>
#include <libc/string.h>


uint32_t nvme_parts[10];


void dev_enum() {

    //create dev directory in root
    vnode_t* dev = new_vnode(&root_node, "dev");
    dev->flags = 0x80000000;
    dev->size = 0;
    dev->fs = &devfs;

    //create zero
    vnode_t* zero = new_vnode(dev, "zero");
    zero->flags = 0;
    zero->size = 0;
    zero->fs = &devfs;

    //create zero
    vnode_t* rand = new_vnode(dev, "rand");
    rand->flags = 0;
    rand->size = 0;
    rand->fs = &devfs;

    //create tty
    init_terminal();
    vnode_t* term = new_vnode(dev, "tty");
    term->flags = 0;
    term->size = 0;
    term->fs = &devfs;


    init_pci();
	struct pci_header_t header = getHeader(detect_nvme());
	uint32_t mmio = (header.BAR0 & 0xFFFFFFF0);

	init_admin(mmio);
	init_ioq();
	create_iocq(1);
	create_iosq(1);
    for (uint32_t i = 0; i < 0xffffff; i++);

    char* nvme_name = kmalloc();
    strcpy(nvme_name, "nvme");
    vnode_t* nvme = new_vnode(dev, nvme_name);
    nvme->flags = 0;
    nvme->size = 0;
    nvme->fs = &devfs;

    //detect mbr
    void* buffer = kmalloc();
    nvme_read(buffer, 8, 0); //load in the first 4kb to buffer
    if (*(uint16_t*)(buffer + 0x1fe) == 0xaa55) {

        //println("MBR detected");
        mbr_pentry_t* partition = buffer + 0x1be;
    
        void* gpt = buffer + 512;
        *(char*)(gpt + 8) = 0;
        if (strcmp((char*)gpt, "EFI PART") == 0) {
            //println("GPT detected");
            uint32_t entry_size = *(uint32_t*)(gpt + 0x54);
            uint32_t no_parts = *(uint32_t*)(gpt + 0x50);

            gpt_pentry_t* pentry = buffer + 1024; //lba 2
            for (uint32_t i = 0; i < 8; i++) {
                //create partition files in dev
                uint32_t used = 0;
                for (uint8_t j = 0; j < 16; j++) {
                    used += (uint8_t)pentry[i].type_GUID[j];
                }
                if (used == 0) continue; //unused entry

                strcpy(nvme_name, "nvme");
                nvme_name[4] = i + 48;
                nvme_name[5] = 0;

                vnode_t* nvme = new_vnode(dev, nvme_name);
                nvme->flags = 0;
                nvme->size = pentry[i].lba_end_lo - pentry[i].lba_start_lo + 1; //in blocks
                nvme->fs = &devfs;
                nvme->fs_instance = i;
                nvme_parts[i] = pentry[i].lba_start_lo;


            }

        } else {
            //partition based on MBR entries
        }


    }
    kfree(nvme_name);
    kfree(buffer);
}

uint8_t dev_read(void* buf, uint32_t count, uint32_t position, vnode_t* vnode) {
    void* dev_read_func;
    uint32_t newpos = position;
    if (strcmp(vnode->name, "tty") == 0) dev_read_func = &sendbuffer;
    else if (strcmp(vnode->name, "nvme") == 0) dev_read_func = &nvme_read;
    else if (strcmp(vnode->name, "nvme5") == 0) {
        dev_read_func = &nvme_read;
        newpos += nvme_parts[5];
    }
    else kpanic("Unknown device");

    uint8_t (*func)(void*, uint32_t, uint32_t, struct vfs_node*) = dev_read_func;
    return (*func)(buf, count, newpos, vnode);
}

void dev_write(void* buf, uint32_t count, uint32_t position, vnode_t* vnode) {
    void* dev_write_func;
    if (strcmp(vnode->name, "tty") == 0) dev_write_func = &print;
    else if (strcmp(vnode->name, "nvme") == 0) kpanic("Atempted to write to nvme");
    else kpanic("Unknown device");

    void (*func)(void*, uint32_t, uint32_t, struct vfs_node*) = dev_write_func;
    (*func)(buf, count, position, vnode);
}