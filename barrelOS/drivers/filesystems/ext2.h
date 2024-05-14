#include <kernel/vfs.h>
#include <stdint.h>

typedef struct ext2_superblock {
    uint32_t no_inodes;
    uint32_t no_blocks;
    uint32_t block_size;
    uint32_t group_block_count;
    uint32_t group_inode_count;

    uint32_t inode_size;

    uint32_t ext2_sig;
} ext2_sb_t;

typedef struct inode {
    uint16_t type_perms;
    uint16_t user_id;
    uint32_t size_lo;
    uint32_t last_access;
    uint32_t creation_time;
    uint32_t last_changed;
    uint32_t deletion_time;
    uint16_t group_id;
    uint16_t no_hardlinks;
    uint32_t no_sectors; //not ext2 blocks
    uint32_t flags;
    uint32_t os_value;

    uint32_t block_pointer[12];

    void* singly_indirect;
    void* doubly_indirect;
    void* triply_indirect;

    uint32_t generation_no;
    uint32_t file_acl; //or reserved
    uint32_t size_hi; //if feature bit set (Directory ACL if directory instead)

    void* fragment;
    char os_value2[12];

} __attribute__((packed)) inode_t;

typedef struct block_group_descriptor {
    uint32_t block_bitmap;
    uint32_t inode_bitmap;
    uint32_t inode_table;

    uint16_t unalloc_blocks;
    uint16_t unalloc_inodes;
    uint16_t no_directories;

    char reserved[14];

} __attribute__((packed)) bgd_t;

typedef struct ext2_directory_entry {
    uint32_t inode;
    uint16_t size;
    uint8_t name_length;
    uint8_t type;
    //might also be name-length hi depending on "directory entires have file type byte"
    //setting in superblock
} __attribute__((packed)) dirent_t;

uint8_t ext2_read(void* buf, uint32_t count, uint32_t position, vnode_t* vnode);
void ext2_write(void* buf, uint32_t count, uint32_t position, vnode_t* vnode);
void ext2_enum(vnode_t* vnode);

inode_t* get_inode(vnode_t* vnode);
void ext2_mount(char* mountee, char* mountpoint);