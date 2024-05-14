#include <stdint.h>
#include <kernel/vfs.h>
#include <drivers/screen.h>
#include <kernel/idt/interrupt_handlers.h>
#include <kernel/vmm.h>
#include <drivers/utils.h>
#include <drivers/utils.h>
#include <drivers/nvme.h>
#include <drivers/utils.h>
#include <drivers/filesystems/tmpfs.h>
#include <drivers/filesystems/ext2.h>
#include <drivers/filesystems/devfs.h>
#include <libc/string.h>

#define FD_TABLE_SIZE 8
#define NULL 0

extern uint32_t active_pid;

fd_t* fd_tables[UINT8_MAX]; //array of pointers

vnode_t root_node;

const fs_t tmpfs = {&tmpread, &tmpwrite, NULL, NULL};
const fs_t devfs = {&dev_read, &dev_write, NULL, NULL};
const fs_t ext2fs = {&ext2_read, NULL, &ext2_enum, &ext2_mount};



void init_vfs() {
    
    mem_set((void*)fd_tables, 0, 256*4); //hvae to zero this manually for some reason
    
    strcpy(root_node.name, "root");
    root_node.flags = 0x80000000;
    root_node.size = 0;
    root_node.dirents = kmalloc();
    root_node.dirents->name[0] = 0; //make first entry null
    root_node.fs = &tmpfs;

    init_tmpfs();
    dev_enum();
}

void mount(char* mountee, char* mountpoint, const fs_t* fs) {
    
    vnode_t* mountee_node = getnode(mountee);
    vnode_t* mountpoint_node = getnode(mountpoint);

    //ensure mountpoint is empty
    if ((mountpoint_node->flags & 0x80000000) == 0) kpanic("can only mount on directory");
    if (mountpoint_node->dirents->name[0] != 0) kpanic("cannot mount unless directory is empty");

    //alter mountpoint vnode fs_t
    mountpoint_node->fs = fs;

    println("Mounting ");
    print(mountee);
    print(" to ");
    print(mountpoint);
    print("\n");


    //call fs-specific mount function
    void (*func)(char*, char*) = mountpoint_node->fs->mount;
    (*func)(mountee, mountpoint);
}


vnode_t* getnode(char* path) {

    //find node corresponding to filepath
    uint8_t i = 0;
    char* currentpath = kmalloc();
    strcpy(currentpath, "");
    vnode_t* currentnode = &root_node;

    char* pathpart_page = kmalloc();
    char* pathpart = pathpart_page;
    strcpy(pathpart, path);
    pathpart = strtok(pathpart, "/");

    //TODO if path doesn't start with /, prepend the working directory?
    
    while(strcmp(currentpath, path) != 0) {
        strcat(currentpath, "/");
        if (pathpart == NULL) kpanic("Critical error in open()");

        navigate:
        if ((currentnode->flags & 0x80000000) == 0) kpanic("Part of path is not a directory");
        
        //tell fs driver to enumerate dirents for current node, if it has a function for it
        if (currentnode->fs->enumdir != NULL) {
            //clear current dirents
            mem_set(currentnode->dirents, 0, 4096);

            void (*func)(vnode_t*) = currentnode->fs->enumdir;
            (*func)(currentnode);
        }

        

        if (strcmp(currentnode->dirents[i].name, pathpart) == 0) {
            currentnode = &currentnode->dirents[i];
            strcat(currentpath, pathpart);
            pathpart = strtok(NULL, "/");
            i = 0;
            continue;
        }

        

        if (strcmp(currentnode->dirents[i].name, "") == 0) {
            if ((/*mode & */0b00000100) == 1) {
                kpanic("file creation not set up yet");
                //has to call fs-specific stuff to create actual files
                //create vnodes along the way by strtoking pathpart, and continuing, should work ok
            }
            else {
                println("could not find ");
                print(pathpart);
                print(" after ");
                print(hexstring(i, 32));
                print(" attempts");
                while(1);//kpanic("File or directory doesn't exist");
            }
        }
        

        i++;
        goto navigate;
    }
    
    kfree(currentpath);
    kfree(pathpart_page);
    return currentnode;
}

vnode_t* new_vnode(vnode_t* parent, char* name) {
    vnode_t* newnode = parent->dirents; //start at first dirent
    while(newnode->name[0] != 0){ //find end of direntries
        if(strcmp(newnode->name, name) == 0) kpanic("file/direcory with that name already exists");
        newnode++;
    }

    strcpy(newnode->name, name);
    (newnode + 1)->name[0] = 0; //make sure next parent dirent is null

    newnode->dirents = kmalloc(); //assign space for this node's dirents
    newnode->dirents->name[0] = 0; //make first entry null

    return newnode;
}

void remove_vnode(struct vfs_node* node) {

}


void readdir(char* path, void* buf) {
    vnode_t* node = getnode(path);
    if (node->flags & 0x80000000 == 0) return; //not a directory
    
    //enumerate
    if (node->fs->enumdir != NULL) {
        //clear current dirents
        mem_set(node->dirents, 0, 4096);

        void (*func)(vnode_t*) = node->fs->enumdir;
        (*func)(node);
    }
    
    //create contents list
    char* contents = kmalloc();
    contents[0] = 0; //reset string
    uint8_t i = 0;
    while(node->dirents[i].name[0] != 0) {
        strcat(contents, node->dirents[i].name);
        strcat(contents, "\n");
        i++;
    }

    strcpy(buf, contents);
    kfree(contents);
}

//open() mode
//bits 7-3 reserved
//bit 2 create if doesn't exist
//bit 1 write
//bit 0 read
uint8_t open(char* path, uint8_t mode) {
    
    //check if active PID has fd_table
    if (fd_tables[active_pid] == 0) {
        fd_tables[active_pid] = kmalloc();
        mem_set((void*)fd_tables[active_pid], 0, 4096);
    }
    fd_t* fd_table = (fd_t*)fd_tables[active_pid];

    vnode_t* vnode = getnode(path);

    //find available opening in table
    uint16_t i;
    for (i = 0; i < FD_TABLE_SIZE; i++) {
        if (fd_table[i].mode == 0) goto create_entry;
    }
    kpanic("File descriptor table is full");

    //call to fs-specific open?

    create_entry:
    fd_table[i].node = vnode;
    fd_table[i].mode = mode;
    fd_table[i].position = 0;

    //return index to table
    return i;
}

//only returns size right now. Later return a struct of info?
uint32_t stat(uint8_t fd) {
    fd_t* fd_table = (fd_t*)(fd_tables[active_pid]);
    return fd_table[fd].node->size;
}

uint32_t seek(uint8_t fd, uint32_t count, uint8_t whence) {
    fd_t* fd_table = (fd_t*)(fd_tables[active_pid]);

    switch (whence) {
        case 0: //beginning of file
        fd_table[fd].position = count;
        break;
        case 1: //current position
        fd_table[fd].position += count;
        break;
        case 2: //end of file
        fd_table[fd].position = fd_table[fd].node->size + count;
        break;
    }

    return fd_table[fd].position;
}

uint8_t read(uint8_t fd, void* buf, uint32_t count) {
    fd_t* fd_table = (fd_t*)(fd_tables[active_pid]);

    if (fd_table[fd].mode == 0) kpanic("Attempted to read from invalid file descriptor");
    if ((fd_table[fd].mode & 0b00000001) == 0) kpanic("Attempted to read without read mode enabled");
    if (fd_table[fd].node->fs->read == NULL) kpanic("File descriptor has no read function");
    if (fd_table[fd].position + count > fd_table[fd].node->size && fd_table[fd].node->size != 0) {
        println("Attempted to read outside size of file");
        println(hexstring(fd_table[fd].position, 32));
        println(hexstring(count, 32));
        println(hexstring(fd_table[fd].node->size, 32));
        while(1);
    }

    uint8_t (*func)(void*, uint32_t, uint32_t, struct vfs_node*) = fd_table[fd].node->fs->read;
    return (*func)(buf, count, fd_table[fd].position, fd_table[fd].node);
}

void write(uint8_t fd, void* buf, uint32_t count) {
    fd_t* fd_table = (fd_t*)(fd_tables[active_pid]);

    if (fd_table[fd].mode == 0) kpanic("Attempted to write to invalid file descriptor");
    if ((fd_table[fd].mode & 0b00000010) == 0) kpanic("Attempted to write without write mode enabled");
    if (fd_table[fd].node->fs->write == NULL) kpanic("File descriptor has no write function");
    if (fd_table[fd].position + count > fd_table[fd].node->size && fd_table[fd].node->size != 0) kpanic("Attempted to write outside size of file");

    void (*func)(void*, uint32_t, uint32_t, struct vfs_node*) = fd_table[fd].node->fs->write;
    (*func)(buf, count, fd_table[fd].position, fd_table[fd].node);
}

void close(uint8_t fd) {
    if (fd_tables[active_pid] == 0) kpanic("Attempted to access fd_table of process without one");
    fd_t* fd_table = (fd_t*)fd_tables[active_pid];
    //make sure not busy or something?
    if (fd_table[fd].mode == 0) kpanic("Attempted to close unopened descriptor");
    fd_table[fd].mode = 0;
}