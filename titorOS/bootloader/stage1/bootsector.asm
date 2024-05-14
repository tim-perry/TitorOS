org 0x7c00
bits 16

_start:
jmp after_bpb
nop

bpb:
times 3-($-$$) db 0
db 0
dw 512
db 0
dw 0
db 0
dw 0
dw 0
db 0
dw 0
dw 18
dw 2
dd 0
dd 0
db 0
db 0
db 0
dd 0
db 0
times 8 db 0

after_bpb:
cli
cld
jmp 0:init
init:
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00
sti

;load stage2
mov cx, 2           ;CHS of first sector to read
mov ax, stage2_size ;number of sectors
mov bx, stage2      ;destination
call read

;protected mode
lgdt [gdt_descriptor]
cli
mov eax, cr0
or eax, 1
mov cr0, eax
jmp CODE32_SEG:initpm

bits 32
initpm:
mov eax, DATA32_SEG
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

;push drive number and call stage2
and edx, 0xFF
push edx
call stage2
jmp hang

;includes
%include "bootloader/stage1/bootlib.asm"
%include "bootloader/stage1/gdt.asm"

;partition table
times 0x1bc-($-$$) db 0
;paritions go here

;magic number
times 0x1fe-($-$$) db 0
dw 0xaa55

;STAGE 2
stage2:
incbin 'build/bootloader/stage2/stage2.bin'
times 512 - (($ - $$) % 512) db 0   ;align bootloader to 512 bytes
stage2_size equ ($ - stage2) / 512  ;get size of stage2