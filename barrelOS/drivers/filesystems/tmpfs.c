#include <drivers/filesystems/tmpfs.h>
#include <stdint.h>
#include <kernel/idt/interrupt_handlers.h>
#include <drivers/utils.h>
#include <kernel/vmm.h>
#include <kernel/vfs.h>
#include <libc/string.h>

void* tmpfile;

void init_tmpfs() {
    tmpfile = kmalloc();

    vnode_t* mnt = new_vnode(&root_node, "mnt");
    mnt->flags = 0x80000000;
    mnt->size = 0;
    mnt->fs = &tmpfs;

    vnode_t* file = new_vnode(&root_node, "file");
    file->flags = 0;
    file->size = 0;
    file->fs = &tmpfs;
}


void tmpread(void* buffer, uint32_t count, uint32_t position) {
    strcpy(buffer, tmpfile+position);
}

void tmpwrite(void* buffer, uint32_t count, uint32_t position) {
    strcpy(tmpfile + position, buffer);
}


void tmpfs_enumdir() {
    
    return;
}