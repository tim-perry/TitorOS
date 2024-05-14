#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <kernel/idt/interrupt_handlers.h>


void syscall(isr_stack_t* stack);

#endif