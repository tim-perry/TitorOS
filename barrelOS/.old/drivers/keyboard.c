#include <stdint.h>
#include <drivers/screen.h>
#include <drivers/utils.h>


static uint8_t convertscan(uint8_t);

extern void keypress(){
	uint8_t key = convertscan(inbyte(0x60));
	uint16_t pos=get_cursor();

	
	if(key==10)
		pos+=80-pos%80;
		set_cursor(pos);

	if(pos==2000){
		scroll();
		pos-=80;
		set_cursor(pos);
	}
	
	if(key>=0x20){//beginning of ascii characters
		putchar(key,-1,-1,0x00);
		set_cursor(pos+1);
	}
	

	else if(key==8){
		set_cursor(pos-1);
		putchar(' ',-1,-1,0x00);
	}
	
	return;
}

uint8_t convertscan(uint8_t scancode){
	switch(scancode){
	case 0x2:return '1';
	case 0x3:return '2';
	case 0x4:return '3';
	case 0x5:return '4';
	case 0x6:return '5';
	case 0x7:return '6';
	case 0x8:return '7';
	case 0x9:return '8';
	case 0xa:return '9';
	case 0xb:return '0';

	case 0xe:return 8;

	case 0x10:return 'Q';
	case 0x11:return 'W';
	case 0x12:return 'E';
	case 0x13:return 'R';
	case 0x14:return 'T';
	case 0x15:return 'Y';
	case 0x16:return 'U';
	case 0x17:return 'I';
	case 0x18:return 'O';
	case 0x19:return 'P';

	case 0x1c:return 10;

	case 0x1e:return 'A';
	case 0x1f:return 'S';
	case 0x20:return 'D';
	case 0x21:return 'F';
	case 0x22:return 'G';
	case 0x23:return 'H';
	case 0x24:return 'J';
	case 0x25:return 'K';
	case 0x26:return 'L';
	case 0x27:return ';';
	
	case 0x2c:return 'Z';
	case 0x2d:return 'X';
	case 0x2e:return 'C';
	case 0x2f:return 'V';
	case 0x30:return 'B';
	case 0x31:return 'N';
	case 0x32:return 'M';
	case 0x33:return ',';
	case 0x34:return '.';
	case 0x35:return '/';
	case 0x39:return ' ';

	default:return 0;
	}
}

































