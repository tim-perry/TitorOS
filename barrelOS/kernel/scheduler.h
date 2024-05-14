#include <stdint.h>
#include <kernel/idt/interrupt_handlers.h>

typedef struct process {
    uint32_t pid;
    uint8_t state;
    uint32_t* page_directory;
    isr_stack_t stack;  //stack of usermode

    void* kstack;
    void* stack_pointer;

    struct process* next;
} process_t;

typedef struct kstack {
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    void* return_addr; //return address on stack
    
} __attribute__((packed)) kstack_t;

void scheduler();

process_t* block();
void unblock(process_t* proc);


void init_scheduler();
void init_proc();
void yield();
void init_kstack(process_t* proc);

void idle();
void proc_status(uint8_t status, isr_stack_t*);

void fork(isr_stack_t* stack);
void exec(char* binary, isr_stack_t* stack);