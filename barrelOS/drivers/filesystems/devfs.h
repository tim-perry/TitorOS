#include <stdint.h>
#include <kernel/vfs.h>

typedef struct mbr_partition_table_entry {
    uint8_t attributes;
    
    //CHS START
    uint8_t cylinder;
    uint8_t head;
    uint8_t sector;

    uint8_t type;

    //CHS END
    uint8_t cylinder_end;
    uint8_t head_end;
    uint8_t sector_end;

    //LBA
    uint32_t lba_start;
    uint32_t length;
} __attribute__((packed)) mbr_pentry_t;

typedef struct gpt_paritition_table_entry {
    char type_GUID[16];
    char partition_GUID[16];

    uint32_t lba_start_lo;
    uint32_t lba_start_hi;

    uint32_t lba_end_lo;
    uint32_t lba_end_hi;

    uint32_t attributes_lo;
    uint32_t attributes_hi;

    char partition_name[72];
} __attribute__((packed)) gpt_pentry_t;

void dev_enum();
uint8_t dev_read(void* buf, uint32_t count, uint32_t position, vnode_t* vnode);
void dev_write(void* buf, uint32_t count, uint32_t position, vnode_t* vnode);