#include <drivers/filesystems/ext2.h>
#include <stdint.h>
#include <kernel/idt/interrupt_handlers.h>
#include <kernel/vmm.h>
#include <drivers/utils.h>
#include <drivers/screen.h>
#include <drivers/utils.h>
#include <kernel/vfs.h>

//NOTE: ext2 talks to disk drivers in sector counts/positions rather than bytes
//buffers are still in bytes as ram in only addressable within 4gb
#define BLOCK_FACTOR 8

ext2_sb_t superblock; //not global, put in table for each instance
bgd_t* bgd_table;




uint8_t ext2_read(void* buf, uint32_t count, uint32_t position, vnode_t* vnode) {
    inode_t* inode = get_inode(vnode);

    uint32_t blocks; //how many 4kb blocks to read
    if (count % 4096 == 0) blocks = count / 4096;
    else blocks = count / 4096 + 1; //have to round up to next block
    
    uint8_t volume = open("/dev/nvme5", 0x1);
    seek(volume, inode->block_pointer[0] * BLOCK_FACTOR, 0);
    read(volume, buf, blocks * BLOCK_FACTOR);

    close(volume);
    kfree(inode);
}


void ext2_write(void* buf, uint32_t count, uint32_t position, vnode_t* vnode) {
    kpanic("Attempted to read ext2");
}

void ext2_enum(vnode_t* vnode) {
    if (vnode->flags & 0x80000000 == 0) return; //if not a directory, do nothing

    inode_t* inode = get_inode(vnode);
    dirent_t* entry = kmalloc();

    uint8_t volume = open("/dev/nvme5", 0x01);
    seek(volume, inode->block_pointer[0] * BLOCK_FACTOR, 0);
    read(volume, entry, 1 * BLOCK_FACTOR); //load page containing dirents

    char* name = kmalloc();

    while(entry->inode != 0) {
        //if entry is itself
        if (entry->inode == vnode->inode) { 
            if (entry->name_length % 4 == 0) {
                entry = (void*)(entry + 1) + entry->name_length;
            } else {
                entry = (void*)(entry + 1) + entry->name_length + (4 - entry->name_length % 4);
            }
            continue; //don't enumerate
        }

        mem_copy((void*)(entry + 1), name, entry->name_length);

        name[entry->name_length] = 0; //null-terminate
        vnode_t* node = new_vnode(vnode, name);
        node->inode = entry->inode;
        node->size = get_inode(node)->size_lo;
        node->fs = &ext2fs;

        switch (entry->type) { //assumes type bit is set (see dirent_t struct definition)
            case 1: //regular file
            node->flags = 0;
            break;
            case 2: //directory
            node->flags = 0x80000000;
            break;
        }

        if (entry->name_length % 4 == 0) {
            entry = (void*)(entry + 1) + entry->name_length;
        } else {
            entry = (void*)(entry + 1) + entry->name_length + (4 - entry->name_length % 4);
        }
    }

    vnode->flags |= 0x40000000;
    kfree(name);
    kfree(inode);
    close(volume);
}

inode_t* get_inode(vnode_t* vnode) {

    uint32_t inode_no = vnode->inode;

    uint32_t block_group = (inode_no - 1) / superblock.group_inode_count; //which block group's inode_table
    uint32_t index = (inode_no - 1) % superblock.group_inode_count; //index in that table

    uint32_t inode_table_block = bgd_table[block_group].inode_table; //get first block of the block group's inode table

    uint32_t inodes_per_block = superblock.block_size / superblock.inode_size;

    inode_table_block += index / inodes_per_block; //which block contains the inode
    uint32_t index_in_block = index % inodes_per_block; //index into that block

    inode_t* inode_table = kmalloc();
    uint8_t volume = open("/dev/nvme5", 0x1); //refer to mount table and fs instance?
    seek(volume, inode_table_block * BLOCK_FACTOR, 0);
    read(volume, inode_table, 1 * BLOCK_FACTOR);

    inode_t* inode = kmalloc();
    mem_copy((void*)inode_table + index_in_block * superblock.inode_size, (void*)inode, superblock.inode_size);


    /*
    println("-----------------");
    println(hexstring(inode_no, 32));
    println(hexstring(superblock.group_inode_count, 32));
    println(hexstring(block_group, 32));
    println(hexstring(index, 32));
    println("-----------------");
    println(hexstring(superblock.block_size, 32));
    println(hexstring(superblock.inode_size, 32));
    println(hexstring(inodes_per_block, 32));
    println("-----------------");
    println(hexstring(inode_table_block, 32));
    println(hexstring(index_in_block, 32));
    println("-----------------");
    println(hexstring(inode->type_perms, 16));
    println(hexstring(inode->block_pointer[0], 32));
    println(hexstring(inode->block_pointer[0] * superblock.block_size, 32));
    */

    kfree(inode_table);
    close(volume);
    return inode; //memory leak here - make sure to kfree the pointer returned by this function
}



void ext2_mount(char* mountee, char* mountpoint) {

    uint8_t mountee_fd = open(mountee, 0x1);
    void* sb = kmalloc();
    seek(mountee_fd, 2, 0); //superblock at lba 2 (1024 bytes)
    read(mountee_fd, sb, 2); //superblock is 2 sectors (1024 bytes) long

    superblock.ext2_sig = *(uint32_t*)(sb + 56);
    if ((superblock.ext2_sig & 0xFFFF) != 0xef53) kpanic("NO SUPERBLOCK FOUND");

    superblock.inode_size = *(uint32_t*)(sb + 88);

    superblock.no_inodes = *(uint32_t*)(sb + 0);
    superblock.no_blocks = *(uint32_t*)(sb + 4);

    superblock.block_size = *(uint32_t*)(sb + 24);
    superblock.block_size = 1024 << superblock.block_size; //4096

    superblock.group_block_count = *(uint32_t*)(sb + 32);
    superblock.group_inode_count = *(uint32_t*)(sb + 40);

    uint16_t no_block_groups = superblock.no_blocks / superblock.group_block_count;

    //BGDT
    bgd_table = kmalloc();
    seek(mountee_fd, 1 * BLOCK_FACTOR, 0); //starts at 0x1000
    read(mountee_fd, bgd_table, 1 * BLOCK_FACTOR);

    vnode_t* mountpoint_node = getnode(mountpoint);
    mountpoint_node->inode = 2;
    mountpoint_node->fs_instance = 0; //INSTANCE OF FS FROM TABLE KEPT HERE
    mountpoint_node->flags = 0x80000000;

    close(mountee_fd);
}