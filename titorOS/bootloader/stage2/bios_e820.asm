extern CODE16_SEG
extern DATA16_SEG
extern CODE32_SEG
extern DATA32_SEG

bits 32
global bios_e820
bios_e820:
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

;setup table destination
mov edi, [esp + 32 + 4]
shr edi, 4
and edi, 0xF000 ;isolate upper bits of destination for es
mov es, edi     ;e820 uses [es:di] so put upper bits of destination in es
mov edi, [esp + 32 + 4]
and edi, 0xFFFF ;mask upper bits

;prep for e820
mov ax, 0
push ax ;counter
xor ebx, ebx
mov edx, 0x534D4150

e820_loop:
mov eax, 0xe820
mov ecx, 24
int 0x15
add di, 24

;increment entry counter
pop ax
inc ax
push ax

;list done if carry is set or ebx is zero
jc e820_exit
cmp ebx, 0
je e820_exit
jmp e820_loop

e820_exit:
xor edx, edx
pop dx ;temporarily store return value

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

mov [esp + 28], edx ;get return value to pop into eax on popad
popad
ret