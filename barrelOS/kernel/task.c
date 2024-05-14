#include <stdint.h>
#include <kernel/task.h>
#include <drivers/utils.h>

void init_tss(void* stack_page) {
    struct tss_entry_t* tss = (struct tss_entry_t*)0xc0007e00;
    mem_set((uint8_t*)tss, 0, sizeof(struct tss_entry_t));

    tss->ss0 = 0x10;
    tss->esp0 = (void*)((uint32_t)stack_page & 0xFFFFF000) + 0xffc; //end of page
}