[bits 32]
[extern kernel]
[extern init_idt]
[extern init_pf_bitmap]
[extern init_vmm]
[extern idle]

global _start
global switch_kstack

_start:
;get memory map here? with ax = 0xe881 bios int 0x15

call init_pf_bitmap
call init_vmm

;flush memory map
mov eax, cr3
mov cr3, eax
;turn on copy-on-write

call init_idt
call kernel

;load tss
mov ax, 0x28
ltr ax

sti

call idle

hang:
jmp hang





;void switch_kstack(void* oldstack, void* newstack);
switch_kstack:

    ;push context to stack
    push ebp
    push edi
    push esi
    push ebx
    push edx
    push ecx

    ;put current esp in oldstack
    mov eax, [esp + 28]
    mov [eax], esp


    ;load new stack pointer
    mov esp, [esp + 32]

    ;pop context from stack
    pop ecx
    pop edx
    pop ebx
    pop esi
    pop edi
    pop ebp

    ;inc BYTE [debug]
    ;cmp BYTE [debug], 2
    ;jne skipdebug
    ;pop eax
    ;jmp $
    ;skipdebug:

    ret


debug:
dd 0