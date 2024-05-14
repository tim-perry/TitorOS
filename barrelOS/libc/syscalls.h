#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

uint8_t open(char* path, uint8_t mode);
void write(uint8_t fd, void* buf, uint32_t count);
void read(uint32_t fd, void* buf, uint8_t count);
void clear();
void* mmap(void* hint, uint32_t length, uint8_t flags);
uint32_t fork();
void exec(char* binary);
void _exit(uint32_t status);

#endif