bits32
global load_gdt
load_gdt:
lgdt [gdt_descriptor]
jmp CODE32_SEG:init32
init32:
mov ax, DATA32_SEG
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax
ret

;Global Descriptor Table
gdt_start:	;currently configured to have 2 segments that cover half of all physical memory each

GDT_NULL:	;mandatory null descriptor
dq 0

real_code:
dw 0xffff	;segment limit (bits 0-15)
dw 0x0000	;segment base (bits 0-15)
db 0x00		;segment base (bits 16-23)
;1st flags	present 1	desc. priv lvl 00	desc. type 1			1001
;type flags	code 1		conforming 0		readable 1	accessed 0	1010
;2nd flags	granularity 0	default 32b 0		64b seg 0	AVL 0	0000
db 10011010b	;1st flags	type flags
db 00001111b	;2nd flags	segment size (bits 16-19)	
db 0x00		;segment base (bits 24-31)

real_data:
dw 0xffff	;segment limit (bits 0-15)
dw 0x0000	;segment base (bits 0-15)
db 0x00		;segment base (bits 16-23)
;1st flags	present 1	desc. priv lvl 00	desc. type 1			1001
;type flags	code 0		conforming 0		readable 1	accessed 0	0010
;2nd flags	granularity 0	default 32b 0		64b seg 0	AVL 0	0000
db 10010010b	;1st flags	type flags
db 00001111b	;2nd flags	segment size (bits 16-19)	
db 0x00		;segment base (bits 24-31)

pmode_code:	;code segment descriptor
dw 0xffff	;segment limit (bits 0-15)
dw 0x0000	;segment base (bits 0-15)
db 0x00		;segment base (bits 16-23)
;1st flags	present 1	desc. priv lvl 00	desc. type 1			1001
;type flags	code 1		conforming 0		readable 1	accessed 0	1010
;2nd flags	granularity 1	default 32b 1		64b seg 0	AVL 0	1100
db 10011010b	;1st flags	type flags
db 11001111b	;2nd flags	segment size (bits 16-19)	
db 0x00		;segment base (bits 24-31)

pmode_data:	;data segment descriptor
dw 0xffff	;semment size (bits 0-15)
dw 0x0000	;segment base (bits 0-15)
db 0x00		;segment base (bits 16-23)
;1st flags	present 1	desc. priv lvl 00	desc. type 1			1001
;type flags	code 0		conforming 0		readable 1	accessed 0	0010
;2nd flags	granularity 1	default 32b 1		64b seg 0	AVL 0	1100
db 10010010b	;1st flag	type flags
db 11001111b	;2nd flags	segment size (bits 16-19)	
db 0x00		;segment base (bits 24-31)

long_code:	;code segment descriptor
dw 0xffff	;segment limit (bits 0-15)
dw 0x0000	;segment base (bits 0-15)
db 0x00		;segment base (bits 16-23)
;1st flags	present 1	desc. priv lvl 00	desc. type 1			1001
;type flags	code 1		conforming 0		readable 1	accessed 0	1010
;2nd flags	granularity 1	default 32b 1		64b seg 0	AVL 0	1010
db 10011010b	;1st flags	type flags
db 10101111b	;2nd flags	segment size (bits 16-19)	
db 0x00		;segment base (bits 24-31)

long_data:	;data segment descriptor
dw 0xffff	;semment size (bits 0-15)
dw 0x0000	;segment base (bits 0-15)
db 0x00		;segment base (bits 16-23)
;1st flags	present 1	desc. priv lvl 00	desc. type 1			1001
;type flags	code 0		conforming 0		readable 1	accessed 0	0010
;2nd flags	granularity 1	default 32b 1		64b seg 0	AVL 0	1010
db 10010010b	;1st flag	type flags
db 10101111b	;2nd flags	segment size (bits 16-19)	
db 0x00		;segment base (bits 24-31)

gdt_end:	;for assembler size calculation

gdt_descriptor:	;descriptors of descriptors ;)
dw gdt_end - gdt_start - 1		;GDT size always less one of true size so that this number is the last valid address in the table
dd gdt_start				;address of GDT

;index into gdt
CODE16_SEG equ real_code - gdt_start
DATA16_SEG equ real_data - gdt_start
CODE32_SEG equ pmode_code - gdt_start
DATA32_SEG equ pmode_data - gdt_start
CODE64_SEG equ long_code - gdt_start
DATA64_SEG equ long_data - gdt_start

global CODE16_SEG
global DATA16_SEG
global CODE32_SEG
global DATA32_SEG
global CODE64_SEG
global DATA64_SEG