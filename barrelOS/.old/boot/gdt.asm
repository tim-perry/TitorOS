;Global Descriptor Table

gdt_start:	;currently configured to have 2 segments that cover half of all physical memory each

gdt_null:	;mandatory null descriptor
dd 0x00000000
dd 0x00000000

gdt_code:	;code segment descriptor
dw 0xffff	;semment size (bits 0-15)
dw 0x0000	;segment base (bits 0-15)
db 0x00		;segment base (bits 16-23)

;1st flags	present 1	desc. priv lvl 00	desc. type 1			1001
;type flags	code 1		conforming 0		readable 1	accessed 0	1010
;2nd flags	granularity 1	default 32b 1		64b seg 0	AVL 0		1100

db 10011010b	;1st flag	type flags
db 11001111b	;2nd flags	segment size (bits 16-19)	

db 0x0		;segment base (bits 24-31)




gdt_data:	;data segment descriptor

dw 0xffff	;semment size (bits 0-15)
dw 0x0000	;segment base (bits 0-15)
db 0x00		;segment base (bits 16-23)

;1st flags	present 1	desc. priv lvl 00	desc. type 1			1001
;type flags	code 0		conforming 0		readable 1	accessed 0	0010
;2nd flags	granularity 1	default 32b 1		64b seg 0	AVL 0		1100

db 10010010b	;1st flag	type flags
db 11001111b	;2nd flags	segment size (bits 16-19)	

db 0x0		;segment base (bits 24-31)





gdt_end:	;for assembler size calculation

gdt_descriptor:	;descriptors of descriptors ;)
dw gdt_end - gdt_start - 1		;GDT size always less one of true size so that this number is the last valid address in the table
dd gdt_start				;address of GDT

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

;times 512-($-$$) db 0

;END GDT SECTOR
