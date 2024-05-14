[bits 32]
[extern main]
[extern init_idt]
global idt_load

_start:
call init_idt
;sti
call main
jmp hang

idt_load:
mov eax, [esp+4]
lidt [eax]
ret

hang:
;int 0x0
jmp hang


