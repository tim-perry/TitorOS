extern CODE16_SEG
extern DATA16_SEG
extern CODE32_SEG
extern DATA32_SEG

bits 32
global bios_read
bios_read:
pushad

;switch to 16-bit segment
jmp CODE16_SEG:init16
init16:
bits 16
mov ax, DATA16_SEG
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

;disable pmode
mov eax, cr0
and al, 0xFE
mov cr0, eax

;switch to real mode
jmp 0x00:initreal
initreal:
xor ax, ax
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

;bios_read(drive, disk_sector, dest, count);
mov edx, [esp + 32 + 4]
mov ecx, [esp + 32 + 8]
mov ebx, [esp + 32 + 12]
mov eax, [esp + 32 + 16]

;load sectors
mov ah, 0x02
int 0x13
cli ;bios interrupt might sti

;return to protected mode
mov eax, cr0
or eax, 1
mov cr0, eax
jmp CODE32_SEG:initpm
bits 32
initpm:
mov ax, DATA32_SEG
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

popad
ret