#ifndef TMPFS_H
#define TMPFS_H

#include <stdint.h>

void init_tmpfs();
void tmpread(void* buffer, uint32_t count, uint32_t position);
void tmpwrite(void* buffer, uint32_t count, uint32_t position);
void tmpfs_enumdir();

#endif