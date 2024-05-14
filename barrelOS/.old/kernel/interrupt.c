#include <stdint.h>
#include <drivers/utils.h>
#include <drivers/screen.h>
#include <drivers/keyboard.h>
#include <drivers/timer.h>

//state of stack
struct regs_t{
	uint32_t ds;					//data segment selector
	uint32_t edi,esi,ebp,esp,ebx,edx,ecx,eax;	//pusha
	uint32_t isrnum,errnum;				//isr number and error
	uint32_t eip,cs,eflags,useresp,ss;		//pushed on interrupt
};


//static function prototypes
static void exception_handler(struct regs_t);
static void pic_handler(struct regs_t);
static void panic(struct regs_t);


extern void interrupt_handler(struct regs_t regs){
	//exceptions
	if(regs.isrnum<=31){
		exception_handler(regs);
		return;
	}
	//pic interrupts
	if(regs.isrnum<=46){
		pic_handler(regs);
		return;
	}
	//unknown interrupt
	panic(regs);
}

static void exception_handler(struct regs_t regs){

	//if there's a specific exception handler, call it and return from here
	//else
	panic(regs);
	
}

static void pic_handler(struct regs_t regs){
	if(regs.isrnum==32)
		tick();
	else if(regs.isrnum==33)
		keypress();
	//more
	else panic(regs);

	//respond to PIC and return
	if(regs.isrnum>=40)
		outbyte(0xa0,0x20);
	outbyte(0x20,0x20);
	return;
}

static void panic(struct regs_t regs){
	//TODO exception number to hex conversion and BSOD colours
	wipe_screen(' ',0x17);
	set_cursor(0);
	uint8_t message[] = "Unhandled cpu exception: 0x";
	uint8_t i = 0;
	while(message[i] != 0){
		putchar(message[i],-1,-1,0x00);
		set_cursor(get_cursor()+1);
		i++;
	}
	putchar((uint8_t)regs.isrnum+30,-1,-1,0x00);
	hang:goto hang;
}
