[bits 32]
[extern interrupt_handler]

isr_generic:
pusha

mov ax, ds
push eax
mov ax, 0x10
mov ds,ax
mov es,ax
mov fs,ax
mov gs,ax

call interrupt_handler

pop eax
mov ds,ax
mov es,ax
mov fs,ax
mov gs,ax

popa
add esp, 8
sti
iret


;normal isr
%macro ISR 1
global isr%1
isr%1:
cli
push byte 0	;push errorcode
push byte %1	;push isr number
jmp isr_generic
%endmacro

;if already errored
%macro ISRE 1
global isr%1
isr%1:
cli
push byte %1	;push isr number
jmp isr_generic
%endmacro

;PIC isrs
%macro IRQ 1
global isr%1
isr%1:
cli
push byte 0	;push errorcode
push byte %1	;push isr number
jmp isr_generic
%endmacro

ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISRE 8
ISR 9
ISRE 10
ISRE 11
ISRE 12
ISRE 13
ISRE 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19
ISR 20
ISR 21
ISR 22
ISR 23
ISR 24
ISR 25
ISR 26
ISR 27
ISR 28
ISR 29
ISR 30
ISR 31

IRQ 32
IRQ 33
IRQ 34
IRQ 35
IRQ 36
IRQ 37
IRQ 38
IRQ 39
IRQ 40
IRQ 41
IRQ 42
IRQ 43
IRQ 44
IRQ 45
IRQ 46
IRQ 47
