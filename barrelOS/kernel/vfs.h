#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <drivers/nvme.h>
#include <drivers/filesystems/tmpfs.h>
#include <drivers/screen.h>

#define NULL 0


typedef struct filesystem {
    void* read;
    void* write;
    void* enumdir; //create/refresh vnodes for children of directory
    void* mount;
} fs_t;


typedef struct vfs_node {
    char name[32];
    uint32_t flags; //bit 31 - directory
    uint32_t size; //only applies for fs files?

    const struct filesystem* fs;
    uint32_t inode;
    uint8_t fs_instance;

    struct vfs_node* parent; //points to single parent
    struct vfs_node* dirents; //points to "null terminating" array of children
} vnode_t;


typedef struct file_descriptor {
    vnode_t* node;
    uint8_t mode;
    uint32_t position;
}fd_t;


extern vnode_t root_node;

extern const fs_t tmpfs;
extern const fs_t devfs;
extern const fs_t ext2fs;

void init_vfs();
void mount(char* mountee, char* mountpoint, const fs_t* fs);
vnode_t* new_vnode(vnode_t* parent, char* name);
vnode_t* getnode(char* path);

void readdir(char* path, void* buf);
uint32_t seek(uint8_t fd, uint32_t count, uint8_t whence);
uint32_t stat(uint8_t fd);
uint8_t open(char* path, uint8_t mode);
void close(uint8_t fd);
void write(uint8_t fd, void* buffer, uint32_t count);
uint8_t read(uint8_t fd, void* buf, uint32_t count);

#endif