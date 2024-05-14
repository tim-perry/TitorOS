#include <stdint.h>
#include <kernel/idt/interrupt_handlers.h>
#include <drivers/screen.h>
#include <drivers/utils.h>
#include <drivers/pci.h>
#include <drivers/utils.h>
#include <drivers/timer.h>
#include <kernel/vmm.h>
#include <kernel/syscall.h>
#include <drivers/keyboard.h>
#include <kernel/scheduler.h>

extern process_t active_proc;
isr_stack_t* isr_stack;

void interrupt_handler(isr_stack_t stack) {
	isr_stack = &stack;

	if (stack.vector == 0x20 || stack.vector == 0x21) {
		outbyte(0x20, 0x20); //tell pic the irq has been handled
	}

	switch(stack.vector) {
		case 0x0e:
		page_fault(stack);
		break;

		case 0x20:
		timer_interrupt();
		break;

		case 0x21:
		keyboard_interrupt(stack);
		break;

		case 0x80:
		syscall(&stack);
		break;

		default:
		panic(stack);
		break;
	}

}

void panic(isr_stack_t stack) {
	//clearscreen();
	println("Unhandled Exception");
	println("Interrupt: ");
	print(hexstring(stack.vector, 8));
	println("Error code: ");
	print(hexstring(stack.error, 32));
	println("Base pointer: ");
	print(hexstring(stack.ebp, 32));
	println("Stack pointer: ");
	print(hexstring((uint32_t)stack.esp, 32));
	println("User stack pointer: ");
	print(hexstring((uint32_t)stack.user_esp, 32));

	println("cs: ");
	print(hexstring(stack.cs, 32));
	println("eip: ");
	print(hexstring((uint32_t)stack.eip, 32));

	void* vaddr;
	__asm__("mov %%cr2, %%eax" : "=a" (vaddr) :);
	println("CR2: ");
	print(hexstring((uint32_t)vaddr, 32));

	while(1);
}

void kpanic(char* message) {
	println("kernel panic");
	println(message);
	while(1);
}

void timer_interrupt() {
	tick();
	scheduler();
}

void keyboard_interrupt(isr_stack_t stack) {
	inbyte(0x60);
	keypress();	//must read input from keyboard for it to send another interrupt
}