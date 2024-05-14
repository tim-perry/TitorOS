#ifndef ELF_H
#define ELF_H
#include <stdint.h>

/*
Impliment from:
https://www.uclibc.org/docs/elf-64-gen.pdf
for 64 bit structures and definitions

Also look for 32 bit versions
Then create elf.c implementation for common tasks

Reference elf.h:
https://github.com/torvalds/linux/blob/master/include/uapi/linux/elf.h
*/

typedef struct {
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;     //architecture
    uint32_t e_version;     //always 1
    uint64_t e_entry;       //entry point of program (0 if pie)
    uint64_t e_phoff;       //offset in elf to program header table
    uint64_t e_shoff;       //offset in elf to section header table
    uint32_t e_flags;
    uint16_t e_ehsize;      //size of this header
    uint16_t e_phentsize;   //program header table entry size
    uint16_t e_phnum;       //program header table entry count
    uint16_t e_shentsize;   //section header table entry size
    uint16_t e_shnum;       //section header table entry count
    uint16_t e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    uint32_t p_type;
    uint32_t p_flags;   //access permissions of segment (relevant to setting up page attributes for userspace processes?)
    uint64_t p_offset;  //offset in elf file to segment bytes
    uint64_t p_vaddr;   //where to load
    uint64_t p_paddr;   //where to load
    uint64_t p_filesz;  //size in elf file of segment bytes
    uint64_t p_memsz;   //size of segment in memory (pad zeroes if larger than filesz)
    uint64_t p_align;
} Elf64_Phdr;

//Segment types
#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_TLS 7

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
} Elf64_Shdr;

//Section Types
#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
//...

typedef struct {
    uint32_t st_name;
    uint8_t st_info;
    uint8_t st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
} Elf64_Sym;

typedef struct {
    uint64_t r_offset;
    uint64_t r_info;
} Elf64_Rel;

typedef struct {
    uint64_t r_offset;
    uint64_t r_info;
    uint64_t r_addend;
} Elf64_Rela;

#define ELF64_R_SYM(i) ((i) >> 32)
#define ELF64_R_TYPE(i) ((i) & 0xffffffff)

#endif