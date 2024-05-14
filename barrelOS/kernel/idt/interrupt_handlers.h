#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdint.h>

typedef struct isr_stack{
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	void* esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	
	uint32_t vector; //interrupt vector
	uint32_t error; //error code

	void* eip;
	uint32_t cs;
	uint32_t eflags;
	void* user_esp;
	uint32_t user_ss;
} __attribute__((packed)) isr_stack_t;

void panic(isr_stack_t stack);
void timer_interrupt();
void keyboard_interrupt(isr_stack_t stack);
void kpanic(char*);

#endif