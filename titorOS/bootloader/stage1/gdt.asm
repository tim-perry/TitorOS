gdt_start:

null:
dq 0

pmode_code:	;code segment descriptor
dw 0xffff
dw 0x0000
db 0x00
db 10011010b
db 11001111b
db 0x00

pmode_data:
dw 0xffff
dw 0x0000
db 0x00
db 10010010b
db 11001111b
db 0x00

gdt_end:

gdt_descriptor:
dw gdt_end - gdt_start - 1
dd gdt_start

;index into gdt
CODE32_SEG equ pmode_code - gdt_start
DATA32_SEG equ pmode_data - gdt_start