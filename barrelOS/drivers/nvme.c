//TODO
//admin_complete() function needs to be polled, or interrupts need to be setup so it is called
//use malloc instead of defines for queue buffers

#include <stdint.h>
#include <drivers/nvme.h>
#include <drivers/screen.h>
#include <drivers/utils.h>
#include <kernel/vmm.h>
#include <drivers/utils.h>

#define BLOCK_SIZE 512 //this needs to be set properly with an admin command (if possible?)
#define QL 0x80
//queue length (power of 2 for circular buffer overflow)
// this multiplied by sizeof command needs to be less than the amount of memory allocated for a queue (currently 1 page, therefore 64 commands can fit)

struct queue_pair qid[2]; //currently just admin and io queue pairs
struct command* command;
uint32_t* doorbells;

void setup_queue(uint8_t q) {
    
    //check if submission queue is full
    if (qid[q].sq_tail - qid[q].sq_head == qid[q].queue_length) {
        kpanic("NVME submission queue is full!");
    }

    //prepare command for construction
    command = qid[q].sq_base + qid[q].sq_tail;
    mem_set((uint8_t*)command, 0, sizeof(struct command));
}

void submit(uint8_t q) {
    //write to doorbell
    qid[q].sq_tail++;
    doorbells[2 * q] = qid[q].sq_tail % qid[q].queue_length;
    qid[q].command_id++; //overflow is okay

    //wait for completion entry
    struct cq_entry* cqe = qid[q].cq_base;
    cqe += qid[q].cq_head;
    while((cqe->status & 0x1) != 1);
    qid[q].cq_head++;
}

//interrupt triggers this
void complete(uint8_t q) {
    struct cq_entry* cqe = qid[q].cq_base + qid[q].cq_head;

    //ensure parity bit is set and/or that queue is not empty (parity bit might only be usful if polling)

    qid[q].sq_head = cqe->sq_head;

    //error handling

    qid[q].cq_head++;
    doorbells[1] = qid[q].cq_head % qid[q].queue_length;
}

//setup admin queue and global variables
void init_admin(uint32_t mmio_base) {
    struct config* config = (struct config*)mmiomalloc((void*)mmio_base);
    doorbells = (uint32_t*)(mmiomalloc((void*)(mmio_base + 0x1000)));    //doorbell registers start at 0x1000 above the mmio_base address
    
    qid[0].queue_length = QL;
    qid[0].sq_base = (struct command*)kmalloc();
    qid[0].cq_base = (struct cq_entry*)kmalloc();
    qid[0].sq_head = 0;
    qid[0].sq_tail = 0;
    qid[0].cq_head = 0;
    qid[0].cq_tail = 0;
    qid[0].command_id = 0;
    
    while(config->csts != 1);
    config->cc = 0;                 //disable controller
    while(config->csts != 0);
    config->aqa = qid[0].queue_length << 16 | qid[0].queue_length;  //length of admin queues
	config->asq1 = getpaddr(qid[0].sq_base);      //asq base
    config->asq2 = 0;
	config->acq1 = getpaddr(qid[0].cq_base);      //acq base
    config->acq2 = 0;
    config->cc = 0x00460001;        //enable controller with 64 byte and 16 byte entries for queues
    while(config->csts != 1);

    mem_set(qid[0].cq_base, 0, sizeof(struct cq_entry) * qid[0].queue_length);
}

//admin command set feature 0x7 for io queues
void init_ioq() {
    setup_queue(0);
    command->dword0 = (qid[0].command_id << 16) | 0x9;
    command->dword10 = 0x7;
    command->dword11 = 0x00010001;
    submit(0);
}

//admin command create io completion queue
void create_iocq(uint8_t q) {
    setup_queue(0);
    qid[q].cq_base = (struct cq_entry*)kmalloc();
    qid[q].queue_length = QL;
    qid[q].cq_head = 0;
    qid[q].cq_tail = 0;

    command->dword0 = (qid[0].command_id << 16) | 0x5;
    command->dptr_1 = getpaddr(qid[q].cq_base);
    command->dword10 = (qid[q].queue_length << 16) | q;
    command->dword11 = 0x1;
    submit(0);

    mem_set((uint8_t*)qid[1].cq_base, 0, sizeof(struct cq_entry) * qid[1].queue_length);
}

//admin command create io submission queue
void create_iosq(uint8_t q) {
    setup_queue(0);
    qid[q].sq_base = (struct command*)kmalloc();
    qid[q].sq_head = 0;
    qid[q].sq_tail = 0;
    qid[q].command_id = 0;

    command->dword0 = (qid[0].command_id << 16) | 0x1;
    command->dptr_1 = getpaddr(qid[q].sq_base);
    command->dword10 = (qid[q].queue_length << 16) | q;
    command->dword11 = (q << 16) | 0x1;

    //associate command set?
    command->dword12 = 0;
    submit(0);
}

//admin identify command stub
void identify(void* destination, uint8_t cns) {
    setup_queue(0);
    command->dword0 = (qid[0].command_id << 16) | 0x6;
    command->dptr_1 = destination;
    command->dword10 = cns;
    submit(0);
}

//IO commands

//source_lo is 512b sector to read from
uint8_t nvme_read(void* destination, uint32_t no_blocks, uint32_t source_lo) {
    //println("nvme reading from source block ");
    //print(hexstring(source_lo, 32));
    
    setup_queue(1);
    command->dword0 = 0x2; //read
    command->nsid = 0x1; //nsid
    command->dptr_1 = getpaddr(destination);
    command->dword12 = no_blocks;
    command->dword10 = source_lo;
    submit(1);
    
    return 0; //only do this if completion queue looks good
}
/*
void write(uint32_t no_blocks) {
    setup_queue(1);
    command->dword0 = 0x2; //read
    command->nsid = 0x1; //nsid
    command->dptr_1 = 
    command->dword12 = no_blocks;
    command->dword10 = 
    submit(1);
}
*/