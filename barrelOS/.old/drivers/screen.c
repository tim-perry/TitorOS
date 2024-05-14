#include <stdint.h>
#include <drivers/screen.h>
#include <drivers/utils.h>
#define VID_MEM_BASE 0xb8000

void scroll(){
	for(uint8_t i=0;i<=24;i++)
		mem_copy((uint8_t*)(VID_MEM_BASE+160*(i+1)),(uint8_t*)(VID_MEM_BASE+160*i),160);

}

uint16_t get_cursor(){	
	outbyte(0x3d4,14);
	uint8_t hbyte = inbyte(0x3d5);
	outbyte(0x3d4,15);
	uint8_t lbyte = inbyte(0x3d5);
	return (((uint16_t)(hbyte))<<8)+((uint16_t)(lbyte));
}

void set_cursor(uint16_t cursorpos){
	uint8_t lbyte=(uint8_t)(0x00FF&cursorpos);
	uint8_t hbyte=(uint8_t)(cursorpos>>8);

	outbyte(0x3d4,14);
	outbyte(0x3d5,hbyte);
	outbyte(0x3d4,15);
	outbyte(0x3d5,lbyte);
	return;
}

void putchar(uint8_t character,int8_t xpos,int8_t ypos,uint8_t colour){
	struct screenchar *charpixel = (struct screenchar*) VID_MEM_BASE;
	uint16_t charpos;
	
	if (xpos>=0 && ypos>=0){				//both pos positive
		charpos = ((uint16_t)ypos)*80+(uint16_t)xpos;}	//convert to index
	else charpos = get_cursor();				//getcursor
	if (character)
		charpixel[charpos].character=character;
	if (colour)
		charpixel[charpos].colour=colour;
	return;
}

//clear=0 will not change any character cells
//clear=' ' will clear the screen of characters
//clear=X will fill the screen with X
//colour=0 will keep the old colour scheme
//colour=0xff will change the colour scheme
void wipe_screen(uint8_t character, uint8_t colour){	
	
	for(int i=0;i<=25;i++)
		for(int j=0;j<=80;j++)
			putchar(character,j,i,colour);
	return;
}


