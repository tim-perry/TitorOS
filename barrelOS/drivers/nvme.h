#include <stdint.h>
#ifndef NVME_H
#define NVME_H

void init_admin(uint32_t);
void init_ioq();
void create_iocq(uint8_t);
void create_iosq(uint8_t);
void identify(void*, uint8_t);
uint8_t nvme_read(void* destination, uint32_t count, uint32_t position);

struct queue_pair {
    struct command* sq_base;
    struct cq_entry* cq_base;
    uint16_t queue_length;

    uint16_t sq_tail;
    uint16_t sq_head;

    uint16_t cq_head;
    uint16_t cq_tail;

    uint16_t command_id;
};

struct config{
    uint32_t cap1; //capabilities
    uint32_t cap2;
    uint32_t vs; //version
    uint32_t intms; //interrupt mask set
    uint32_t intmc; //interrupt mask clear
    uint32_t cc; //controller config
    uint32_t reserved;
    uint32_t csts; //controller status
    uint32_t nssr; //NVM subsystem reset
    uint32_t aqa; //Admin queue attributes
    struct command* asq1; //Admin submission queue base address
    struct command* asq2;
    struct cq_entry* acq1; //Admin completion queue base address
    struct cq_entry* acq2;
}__attribute__((packed));

struct command {
    uint32_t dword0;
    uint32_t nsid; //namespace ID
    uint32_t dword2;
    uint32_t dword3;
    void* mptr_lo; //metadata pointer
    void* mptr_hi;
    void* dptr_1;
    void* dptr_2;
    void* dptr_3;
    void* dptr_4;
    uint32_t dword10;
    uint32_t dword11;
    uint32_t dword12;
    uint32_t dword13;
    uint32_t dword14;
    uint32_t dword15;

} __attribute__((packed));

struct cq_entry {
    uint32_t dword0;
    uint32_t dword1;
    uint16_t sq_head;
    uint16_t sq_id;
    uint16_t command_id;
    uint16_t status;
} __attribute__((packed));

#endif