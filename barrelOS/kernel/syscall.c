#include <stdint.h>
#include <kernel/syscall.h>
#include <drivers/screen.h>
#include <drivers/utils.h>
#include <drivers/screen.h>
#include <kernel/scheduler.h>
#include <kernel/vfs.h>
#include <kernel/vmm.h>

void syscall(isr_stack_t* stack) {

    switch (stack->edx) {
        case 0: //open file
        stack->eax = open((char*)stack->eax, stack->ebx);
        break;

        case 1: //write to file
        write(stack-> ebx, (void*)stack->eax, stack->ecx);
        break;

        case 2: //read from file
        read(stack->ebx, (void*)stack->eax, stack-> ecx);
        break;

        case 3: //clearscreen
        clearscreen();
        break;

        case 4: //mmap
        stack->eax = (uint32_t)mmap((void*)stack->eax, stack->ebx, stack->ecx);
        break;

        case 5: //fork
        fork(stack);
        break;

        case 6: //exec
        exec((char*)stack->eax, stack);
        break;

        case 7: //wait
        //check if a child is zombied
        //if yes, clean up the zombie and return
        //if no, block and unblock when a child terminates
        break;

        default:
        kpanic("invalid syscall"); //invalid syscall
    }

}