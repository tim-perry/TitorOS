org 0x7c00
[bits 16]

KERNEL_OFFSET equ 0x200000
KERNEL_SIZE equ 48

PDT_OFFSET equ 0x300000
NO_TABLES equ 3

begin:
;zero segment registers
mov ax, 0
mov ds, ax
mov ss, ax

mov [drive], dl

;setup stack
mov bp, 0x8000
mov sp, bp

;switch to 32 bit protected
cli
lgdt [gdt_descriptor]
mov eax, cr0
or eax, 0x1
mov cr0, eax
jmp unreal

unreal:
mov bx, DATA_SEG
mov ds, bx
mov es, bx
and al, 0xFE
mov cr0, eax
mov ax, 0
mov ds, ax
mov es, ax
sti

;load kernel
mov al, 2 + 8 * NO_TABLES ;first kernel sector
mov ah, KERNEL_SIZE ;number of sectors
mov edx, KERNEL_OFFSET ;where to shoot sectors to
call shoot

;load PDT
mov al, 2 ;source sector
mov ah, 8 * NO_TABLES
mov edx, PDT_OFFSET
call shoot

jmp initpm

hang:
jmp hang

drive:
db 0xff
message:
db 0x0a, 0x0d, "Press to boot BarrelOS", 0x0a, 0x0d, 0

%include "boot/stdio.asm"
%include "boot/diskio.asm"
%include "boot/gdt.asm"

initpm:
mov ax, message
call print
call getchar

cli
mov eax, cr0
or eax, 0x1
mov cr0, eax
jmp CODE_SEG:initpaging

[bits 32]
initpaging:
mov eax, PDT_OFFSET ;page directory address
mov cr3, eax

mov eax, cr0
or eax, 0x80000000
mov cr0, eax

newstack:
mov ax, DATA_SEG
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ebp, 0xc01ffffc
mov esp, ebp

;put gdt in higher half
lgdt [higher_gdt_descriptor]

jmp CODE_SEG:0xC0000000 + KERNEL_OFFSET


times 0x1bc-($-$$) db 0 ;pad up to beginning of partition table
dw 0 ;reserved
%include "boot/partitions.asm"
times 0x1fe-($-$$) db 0 ;pad up to boot signature
dw 0xaa55
