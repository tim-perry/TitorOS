#include <stdint.h>
#include <drivers/timer.h>
#include <drivers/screen.h>
#include <drivers/utils.h>
#include <kernel/vmm.h>
#include <kernel/task.h>
#include <kernel/scheduler.h>
#include <kernel/vfs.h>

void kernel() {
	init_vfs();
	init_scheduler();
	
	//setup init memory map
	mmap(0x00, 4096, 1);
	mmap((void*)0xbffff000, 4096, 1);

	mount("/dev/nvme5", "/mnt", &ext2fs);
	uint8_t shell = open("/mnt/shell.bin", 0x1);
	read(shell, 0, stat(shell));
	close(shell);

	clearscreen();
	init_timer(11930); //sets timer to trigger IRQ 0 every 10 ms
}
