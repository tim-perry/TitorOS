#include <stdint.h>
#include <drivers/utils.h>
#include <drivers/screen.h>
#include <kernel/idt/interrupt_handlers.h>
#include <kernel/vmm.h>
#include <drivers/utils.h>
#include <kernel/vfs.h>
#include <kernel/scheduler.h>

#define VID_MEM_BASE 0xc00b8000

struct __attribute__((packed)) pixel {
	char character;
	uint8_t color;
};

uint8_t screencolor;
void* framecopy;


void setpixel(uint16_t index, char character, uint8_t color) {
	struct pixel* pixel = (struct pixel*)(VID_MEM_BASE + 2 * index);
	pixel->character = character;
	pixel->color = color;
}

void setcolor(uint8_t color) {
	
	struct pixel* screen = (struct pixel*) VID_MEM_BASE;
	screencolor = color;
	for (int i = 0; i < 2000; i++) {
		screen[i].color = screencolor;
	}
	return;
	
}

void clearscreen() {
	
	struct pixel* screen = (struct pixel*)VID_MEM_BASE;
	for (int i = 0; i < 2000; i++) {
		screen[i].character = ' ';
	}

	setcursor(0);
	//clear buffer?
}

void setcursor(uint16_t position) {
	
	uint8_t hi = (position >> 8) & 0xFF;
	uint8_t lo = position & 0xFF;
	outbyte(0x3d4, 14);
	outbyte(0x3d5, hi);
	outbyte(0x3d4, 15);
	outbyte(0x3d5, lo);
}


uint16_t getcursor() {
	uint8_t hi;
	uint8_t lo;
	outbyte(0x3d4, 14);
	hi = inbyte(0x3d5);
	outbyte(0x3d4, 15);
	lo = inbyte(0x3d5);
	uint16_t position;
	position = hi*256 + lo;
	return position;
}

void print(char* string) {
	
	for(int i = 0; string[i] != 0; i++) {
		if (string[i] == '\n') {
			setcursor(getcursor() + (80 - getcursor()%80));
			if (getcursor() >= 80*25) scroll();
			continue;
		}
		setpixel(getcursor(), string[i], screencolor);
		setcursor(getcursor() + 1);
		if (getcursor() >= 80*25) scroll();
	}
	return;
}


void println(char* string) {
	
	if (getcursor() % 80 != 0) {
		setcursor(getcursor() + (80 - getcursor()%80));
		if (getcursor() >= 80*25) scroll();
	}
	print(string);
	return;
	
}

void scroll() {
	uint16_t cursor_pos = getcursor();
	mem_copy((void*)VID_MEM_BASE, framecopy, 2*25*80);
	clearscreen();
	mem_copy(framecopy + 2 * 80, (void*)VID_MEM_BASE, 2 * 24 * 80);
	setcursor(cursor_pos - 80);
}












struct inbuffer {
    uint8_t* start;
    uint8_t length;
};

struct inbuffer buffer;
uint8_t buffered;

process_t** queue_head;
process_t** queue_tail;
extern process_t* active_proc;

void init_terminal() {
	buffer.length = 0;
	buffer.start = kmalloc();
	
	buffered = 0;
	queue_head = kmalloc();
	queue_tail = queue_head;

	framecopy = kmalloc();
	setcolor(0x60);
	clearscreen();
}


void keybuffer(uint8_t key) {

    switch (key) {

    case (0x08): //backspace
    if (buffer.length != 0) {
		buffer.length--;
    	setcursor(getcursor() - 1);
    	setpixel(getcursor(), ' ', screencolor);
	}
	//else bell? :)
    break;

    case (0x0A): //newline
	complete_buffer();
    break;
    
    case (0x1B): //escape
    buffer.length = 0;
	clearscreen();
	break;

    default:
    *(buffer.start + buffer.length) = key;
    buffer.length++;
	setpixel(getcursor(), key, screencolor);
	setcursor(getcursor() + 1);
	if (getcursor() >= 80*25) scroll();
    break;
    }

}


void complete_buffer() {
	//if queue is empty, ignore
	if (queue_head == queue_tail) return;
	
	*(buffer.start + buffer.length) = 0;
	buffer.length++;

	unblock(*queue_head);
	queue_head++;

    setcursor(getcursor() + (80 - getcursor()%80));
	if (getcursor() >= 80*25) scroll();
}

uint8_t sendbuffer(void* destination, uint8_t count) {
	
	//add to unblock queue
	*queue_tail = active_proc;
	queue_tail++;
	block();

    if (buffer.length > count && count != 0) kpanic("sendbuffer too large");
    mem_copy(buffer.start, destination, buffer.length);
    buffer.length = 0;
    buffered = 0;
}