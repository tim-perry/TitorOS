#include <stdint.h>
#include <drivers/utils.h>
#include <kernel/idt/idt.h>

idt_entry_t idt_entry[256]; //should this be global on in kernel heap?
extern uint32_t isrs[256];

void init_idt() {
	init_pic();

	for (int i = 0; i < 256; i++) {
		idt_entry[i].base_hi = (isrs[i] >> 16) & 0xFFFF;
		idt_entry[i].base_lo = isrs[i] & 0xFFFF;
		idt_entry[i].segment = 0x0008;
		idt_entry[i].reserved = 0;
		idt_entry[i].flags = 0b1000 << 4 | 0xE;
		//FLAGS
		//bit 7		present
		//bit 6-5	dpl
		//bit 4 	0
		//bit 3-0 	gate type
	}

	//system calls
	idt_entry[0x80].flags = 0b1110 << 4 | 0xE;

	//load idt
	struct idt_descriptor idt_descriptor = {0x7ff, (uint32_t)idt_entry};
	asm volatile ("lidt %0" : : "m"(idt_descriptor));
}

void init_pic() {
	//sending 0x11 to the command port begins the setup process where 3 sequential "words" are sent to the data port:
	//offset
	//master/slave wiring
	//additional information about environment

	outbyte(0x20, 0x11);	//begin initialisation
	outbyte(0xa0, 0x11);	//begin initialisation
	outbyte(0x21, 0x20);	//master offset
	outbyte(0xa1, 0x28);	//slave offset
	outbyte(0x21, 0x04);	//tell master PIC there is a slave at IRQ 2 (0b00000100)
	outbyte(0xa1, 0x02);	//tell slave PIC its cascade identity?? (0b00000010)
	outbyte(0x21, 0x01);	//8086/88 (MCS-80/85) mode ???
	outbyte(0xa1, 0x01);	//8086/88 (MCS-80/85) mode ???
	
	//Configure IRQ mask
	//1 means ignore
	//Layout, in reverse order: (timer is on least significant bit)
	//Timer Keyboard PIC2 COM2 COM1 LPT2 Floppy LPT1
	outbyte(0x21, 0b11111100);
	outbyte(0xa1, 0b11111111);
}