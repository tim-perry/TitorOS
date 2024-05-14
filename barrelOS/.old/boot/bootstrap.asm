[org 0x7c00]
[bits 16]

;load prompt
mov ax,continue
call print
call getchar

;read disk
call read

mov ax, continue
call print
;jmp hang



;Set up GDT for switching to 32 bit protected mode
cli
lgdt [gdt_descriptor]
mov eax,cr0
or eax,0x1
mov cr0,eax
jmp CODE_SEG:initpm	;kernel code needs to be loaded

;libraries, hang, and data
%include 'boot/diskio.asm'
%include 'boot/stdio.asm'
%include 'boot/gdt.asm'
hang:
jmp hang
;messages
continue:
db "Press any key to load OS...",0x0a,0x0d,0

;setup protected mode
[bits 32]
initpm:
mov ax,DATA_SEG
mov ds,ax
mov ss,ax
mov es,ax
mov fs,ax
mov gs,ax
mov ebp,0x90000	
mov esp,ebp
jmp CODE_SEG:KERNEL_OFFSET


times 510-($-$$) db 0
dw 0xaa55

