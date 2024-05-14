#include <stdint.h>
#include <drivers/utils.h>

//declare assembly interupt service routines
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();
extern void isr40();
extern void isr41();
extern void isr42();
extern void isr43();
extern void isr44();
extern void isr45();
extern void isr46();
extern void isr47();

extern void idt_load(uint32_t);


//Structure definitions
struct __attribute__((packed)) idt_entry_t{
	uint16_t base_lo;
	uint16_t selector; //segment selector?
	uint8_t reserved; //must always be zero
	uint8_t flags;
	uint16_t base_hi;
};

//try typedefing this and change declaration later (experiment)
struct __attribute__((packed)) idt_descriptor_t{
	uint16_t limit;	//size-1?
	uint32_t base;
};




struct idt_entry_t idt_entry[256];

void idt_setup(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags){
	idt_entry[num].base_hi = base & 0xFFFF;
	idt_entry[num].base_lo = base>>16 & 0xFFFF;
	idt_entry[num].selector = sel;
	idt_entry[num].reserved = 0;
	idt_entry[num].flags = flags;
}


//Initiate table
void init_idt(){

	//Remap the irq table
	outbyte(0x20, 0x11);
	outbyte(0xa0, 0x11);
	outbyte(0x21, 0x20);	//master offset
	outbyte(0xa1, 0x28);	//slave offset
	outbyte(0x21, 0x04);	
	outbyte(0xa1, 0x02);
	outbyte(0x21, 0x01);
	outbyte(0xa1, 0x01);
	outbyte(0x21, 0xFc);	//mask
	outbyte(0xa1, 0xff);	//mask

	//Setup idt
//	idt_entry[0]=(struct idt_entry_t){(uint32_t)isr0&0xFFFF,0x08,0x00,0x8E,(uint32_t)isr0>>16&0xFFFF};
	idt_setup(0,(uint32_t)isr0,0x08,0x8E);		//divide by zero
	idt_setup(1,(uint32_t)isr1,0x08,0x8E);
	idt_setup(2,(uint32_t)isr2,0x08,0x8E);
	idt_setup(3,(uint32_t)isr3,0x08,0x8E);
	idt_setup(4,(uint32_t)isr4,0x08,0x8E);
	idt_setup(5,(uint32_t)isr5,0x08,0x8E);
	idt_setup(6,(uint32_t)isr6,0x08,0x8E);
	idt_setup(7,(uint32_t)isr7,0x08,0x8E);
	idt_setup(8,(uint32_t)isr8,0x08,0x8E);		//double fault
	idt_setup(9,(uint32_t)isr9,0x08,0x8E);
	idt_setup(10,(uint32_t)isr10,0x08,0x8E);
	idt_setup(11,(uint32_t)isr11,0x08,0x8E);
	idt_setup(12,(uint32_t)isr12,0x08,0x8E);
	idt_setup(13,(uint32_t)isr13,0x08,0x8E);	//general protection
	idt_setup(14,(uint32_t)isr14,0x08,0x8E);
	idt_setup(15,(uint32_t)isr15,0x08,0x8E);
	idt_setup(16,(uint32_t)isr16,0x08,0x8E);
	idt_setup(17,(uint32_t)isr17,0x08,0x8E);
	idt_setup(18,(uint32_t)isr18,0x08,0x8E);
	idt_setup(19,(uint32_t)isr19,0x08,0x8E);
	idt_setup(20,(uint32_t)isr20,0x08,0x8E);
	idt_setup(21,(uint32_t)isr21,0x08,0x8E);
	idt_setup(22,(uint32_t)isr22,0x08,0x8E);
	idt_setup(23,(uint32_t)isr23,0x08,0x8E);
	idt_setup(24,(uint32_t)isr24,0x08,0x8E);
	idt_setup(25,(uint32_t)isr25,0x08,0x8E);
	idt_setup(26,(uint32_t)isr26,0x08,0x8E);
	idt_setup(27,(uint32_t)isr27,0x08,0x8E);
	idt_setup(28,(uint32_t)isr28,0x08,0x8E);
	idt_setup(29,(uint32_t)isr29,0x08,0x8E);
	idt_setup(30,(uint32_t)isr30,0x08,0x8E);
	idt_setup(31,(uint32_t)isr31,0x08,0x8E);
	idt_setup(32,(uint32_t)isr32,0x08,0x8E);	//timer
	idt_setup(33,(uint32_t)isr33,0x08,0x8E);	//keyboard
	idt_setup(34,(uint32_t)isr34,0x08,0x8E);	//slave PIT
	idt_setup(35,(uint32_t)isr35,0x08,0x8E);
	idt_setup(36,(uint32_t)isr36,0x08,0x8E);
	idt_setup(37,(uint32_t)isr37,0x08,0x8E);
	idt_setup(38,(uint32_t)isr38,0x08,0x8E);
	idt_setup(39,(uint32_t)isr39,0x08,0x8E);
	idt_setup(40,(uint32_t)isr40,0x08,0x8E);
	idt_setup(41,(uint32_t)isr41,0x08,0x8E);
	idt_setup(42,(uint32_t)isr42,0x08,0x8E);
	idt_setup(43,(uint32_t)isr43,0x08,0x8E);
	idt_setup(44,(uint32_t)isr44,0x08,0x8E);
	idt_setup(45,(uint32_t)isr45,0x08,0x8E);
	idt_setup(46,(uint32_t)isr46,0x08,0x8E);
	
	struct idt_descriptor_t idt_descriptor = { 8*256-1, (uint32_t)&idt_entry };
	idt_load((uint32_t)&idt_descriptor);
	return;
}
