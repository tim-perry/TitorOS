;Segment Labels
KCODE equ kernel_code - gdt_start
KDATA equ kernel_data - gdt_start
global KCODE
global KDATA

;Global Descriptor Table
gdt_start:

gdt_null:
dq 0

kernel_code:
times 5 db 0x00
db 10011010b    ;access byte
db 00100000b    ;flags
db 0x00

kernel_data:
times 5 db 0x00
db 10010010b    ;access byte
db 00000000b    ;flags
db 0x00

gdt_end:

;GDT Descriptor
gdt_descriptor:
dw gdt_end - gdt_start - 1		;limit
dq gdt_start                    ;base

;Load GDT
bits64
global load_gdt
load_gdt:
push KCODE
push load_cs
lgdt [gdt_descriptor]
retfq
load_cs:
mov ax, KDATA
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax
ret