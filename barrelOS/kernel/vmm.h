#include <stdint.h>
#include <kernel/idt/interrupt_handlers.h>

void init_vmm();
void* kmalloc();
void kfree(void*);
void* mmiomalloc(void*);
uint32_t* page_entry(void*);
void page_fault(isr_stack_t);
void* getpaddr(void*);
void setallow(uint8_t allow);
void* mmap(void* hint, uint32_t length, uint8_t flags);