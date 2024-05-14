;fix line 12, 13, 29

read: ;takes absolute offset of sector to read on al
pusha

;setup
mov ah, 0x02 ;read mode
mov dl, [drive] ;drive
mov ch, 0x00 ;cylinder
mov dh, 0x00 ;head

sethead:
cmp al, 63
jle setsector
add dh, 1
sub al, 63
jmp sethead


setsector:
mov cl, al ;sector

mov al, 1 ;no_sectors to read

;location in memory to load to (es:bx) FFFF:0010
mov bx, 0
mov es, bx
mov bx, 0x8000

;Transfer
int 0x13

;error checking
jc disk_error ;if carry bit was set, there was an error

cmp ah, 0 ;ah should be 0 on success
jne disk_error

cmp al, 1 ;check that correct number of sectors were loaded
jne disk_error

;Read was successful
popa
ret

disk_error:
mov ax, disk_error_message
call print
jmp hang

disk_error_message:
db 0x0a, 0x0d, "Error reading disk", 0x0a, 0x0d, 0




;offset of first source sector on al
;number of sectors on ah
;destination on edx
shoot:
pusha

mov bh, 0 ;number of sectors read
readloop:
call read ;loads sector into 0x8000

;copy it to high memory
mov ecx, 512 ;repetitions
mov esi, 0x8000 ;source
mov edi, edx ;destination

mov bl, bh ;offset destination by number of sectors read so far
offset:
cmp bl, 0
je copy
add edi, 512 ;offset
dec bl
jmp offset

copy:
rep a32 o32 movsb

inc bh ;number of sectors read
inc al ;offset of next sector to read
cmp bh, ah ;read last sector?
jne readloop

popa
ret