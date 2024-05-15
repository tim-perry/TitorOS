## BarrelOS
BarrelOS is an earlier version of my hobby OS, which has far more features but has a rather messy codebase. Features are progressively being ported to TitorOS with better implementations.

### Feature List
Boot sector that:  
    Sets up GDT and PDT (page tables)  
    Switches the CPU to 32-bit protected mode  
    Loads the kernel into memory  

Interrupt handling  
Physical memory manager  
Virtual memory manager (called on page fault)  
Virtual Filesystem that allows for mounting devices and file i/o  
Scheduler for process switching  
Basic screen and keyboard drivers for user i/o  
PCI, NVMe and Ext2 drivers for accessing actual files on storage media  
Basic unix-like syscall funcionality including read, write, mmap, fork, and exec  