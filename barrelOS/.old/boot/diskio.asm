KERNEL_OFFSET equ 0x8000
SECTOR_COUNT equ 18 ;size of kernel.img/512 - 1

read:
pusha

;load kernel at 1MB, aka 0x100000, aka 0xFFFF:0x0010
mov bx,0x8000
mov es,bx
mov bx,0x0000
;mov bx,0x0010

;transfer setup
mov ah,0x02		;read mode
mov al,SECTOR_COUNT	;no. of sectors
mov ch,0x00		;cylinder
mov cl,0x02		;sector
mov dh,0x00		;head
mov dl,0x80		;drive

;Transfer and error checking
int 0x13
jc diskerror
cmp al,SECTOR_COUNT
je loaded

mov ax,error2
call print
jmp hang
diskerror:
mov ax,error1
call print
jmp hang

;Error Messages
error1:
db "Disk read error",0x0a,0x0d,0
error2:
db "Sector read error",0x0a,0x0d,0

loaded:
popa
ret
