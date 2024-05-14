#include <drivers/screen.h>
#include <drivers/utils.h>

void init_timer(uint32_t);

void main(){

	wipe_screen(' ',0x70);//pass in 0 or ' ' as character to keep or to clear. pass in either 0x00 or a colour to either keep colour or change colour
	set_cursor(0x0);

	uint8_t string[]={"This is literally all the OS does right now"};
	for(int i=0;string[i];i++){
		putchar(string[i],-1,-1,0x00);
		set_cursor(get_cursor()+1);
	}


	//putchar('A',10,10,0x00);
	init_timer(1);
	

	return;
}

void init_timer(uint32_t freq){
	uint32_t divisor = 1193180/freq;
	outbyte(0x43,0x36);
	uint8_t l = (uint8_t)(divisor & 0xFF);
	uint8_t h = (uint8_t)((divisor>>8)&0xFF);
	outbyte(0x40,l);
	outbyte(0x40,h);
}

/* 0, black
 * 1, blue
 * 2, green
 * 3, cyan
 * 4, red
 * 5, pink
 * 6, orange
 * 7, white
 */
