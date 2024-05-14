#include <stdint.h>
#include <kernel/scheduler.h>
#include <drivers/screen.h>
#include <drivers/utils.h>
#include <drivers/screen.h>
#include <drivers/utils.h>
#include <kernel/vmm.h>
#include <kernel/vfs.h>
#include <kernel/task.h>

#define READY 0
#define WAITING 1

extern fd_t* fd_tables[UINT8_MAX]; //array of pointers
extern isr_stack_t* isr_stack;
extern uint32_t int_depth;
extern void firstret();
extern void* switch_kstack(void*, void*);

uint32_t active_pid; //used for stack aligner
process_t* active_proc;
process_t* old_proc;
process_t idle_proc; //first node in linked list

void scheduler() {
    
    //select next process
    old_proc = active_proc;
    do {
        active_proc = active_proc->next;
    } while (active_proc->state != READY);
    active_pid = active_proc->pid;
    
    //switch page directory
    void* directory = getpaddr(active_proc->page_directory);
	asm volatile("movl %0, %%cr3" : : "r"(directory) : "memory");

    //setup tss for interrupts that happen in this new process
    init_tss(active_proc->kstack);

    //switch
    switch_kstack(&(old_proc->stack_pointer), active_proc->stack_pointer);
}

process_t* block() {
    active_proc->state = WAITING;
    scheduler();
    return active_proc;
}

void unblock(process_t* proc) {
    proc->state = READY;
}



void init_kstack(process_t* proc) {
    proc->kstack = kmalloc();
    isr_stack_t* frame = proc->kstack + 0x1000 - sizeof(isr_stack_t) - 4;
    *frame = proc->stack;

    kstack_t* kframe = (void*)frame - sizeof(kstack_t);

    kframe->return_addr = &firstret; //return address on stack
    kframe->ebp = 0xb; //proc->kstack + 0xffc;
    kframe->edi = 0xe;
    kframe->ecx = 0xc;

    proc->stack_pointer = (void*)kframe;
}


void idle() {
    halt:
    asm("hlt");
    goto halt;
}

void init_scheduler() {
    //initialize idle process
    idle_proc.pid = 0;
    idle_proc.page_directory = (void*)0xc0300000;
    idle_proc.next = &idle_proc;
    idle_proc.kstack = (void*)0xc01ff000;

    active_pid = 0;
    active_proc = &idle_proc;

    init_tss((void*)0xc01ff000);

    init_proc();   
}

void init_proc() {
    process_t* init = kmalloc();

    init->stack.eax = 0xcafebabe;
    init->stack.eip = 0;
    init->stack.cs = 0x1b;
    init->stack.eflags = 0x282;
    init->stack.user_esp = (void*)0xbffffffc;
    init->stack.user_ss = 0x23;
    init->state = READY;
    init->pid = 1;

    init->page_directory = idle_proc.page_directory;
    init_kstack(init);

    idle_proc.next = init;
    init->next = &idle_proc;
}













void fork(isr_stack_t* stack) {
    //create new process struct
    process_t* new = kmalloc();
    new->state = active_proc->state;
    new->stack = *isr_stack;
    new->stack.eflags = 0x282;
    new->pid = 2; //next available pid?

    //clone fd table
    fd_tables[new->pid] = kmalloc();
    mem_copy(fd_tables[active_pid], fd_tables[new->pid], 4096);
    
    //create copy of page directory, with zeroed write bit
    uint32_t* newdir = kmalloc();
    mem_copy(active_proc->page_directory, newdir, 4096);
    for (uint16_t i = 0; i < 1024; i++) {
        newdir[i] &= 0xFFFFFFFD; //turn off write bit
    }
    
    //point directory to itself in last entry
    newdir[1023] = (uint32_t)getpaddr(newdir) | 0b100000000011;
    new->page_directory = newdir;

    //link it into the list
    new->next = active_proc->next;
    active_proc->next = new;

    //return pid of new process to parent
    isr_stack->eax = new->pid;
    //return 0 to child
    new->stack.eax = 0;

    //new kernel stack (4k)
    init_kstack(new);
}

void exec(char* binary, isr_stack_t* stack) {
    //println("exec in pid");
    //print(hexstring(active_pid, 32));

    uint8_t fd = open(binary, 0x1);
    uint32_t size = stat(fd);

    //fresh page directory (up to kernel space)
    for (uint16_t i = 0; i < 0x300; i++) {
        active_proc->page_directory[i] = 0;
    }
    asm volatile("movl %%cr3, %%eax\n\t movl %%eax, %%cr3" : : : "eax", "memory");

    //map code area
    mmap(0x0000, size, 1); //flag 1 means take hint literally (put at address zero)
    //map stack area
    mmap((void*)0xbffff000, 4096, 1);

    //load in program
    read(fd, 0x0000, size);
    close(fd);

    //load "fresh" register context
    stack->eip = 0;
    stack->user_esp = (void*)0xbffffffc;
    //if elf, set eip to entry point?
}