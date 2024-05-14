[bits 32]
[extern interrupt_handler]
[extern active_pid]
global isrs
global firstret
global int_depth


int_depth: ;store interrupt depth for stack alignment
dd 0

debug:
dd 0

isrs:
%assign vector 0
%rep 256
dd isr%+vector
%assign vector vector + 1
%endrep

generic_isr:
pusha

mov ax, 0x10
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

inc BYTE [int_depth]

cmp DWORD [active_pid], 0
je pad
;didn't jump, therefore we came from usermode
padret:

call interrupt_handler
firstret: ;used for new process kernel stacks

cmp BYTE [int_depth], 1
jg depad

cmp DWORD [active_pid], 0
je depad

;returning to usermode, so set up data segments
mov eax, 0x23
mov ds, eax
mov es, eax
mov fs, eax
mov gs, eax

depadret:
dec BYTE [int_depth]
popa
add esp, 8

;inc BYTE [debug]
;cmp BYTE [debug], 34
;jne skipdebug
;pop eax
;jmp $
;skipdebug:

iret

pad:
pusha

;repetitions
mov ecx, 4 * 13 + 4 * 8
;source
mov esi, esp
;destination
mov edi, esp
sub edi, 8
rep a32 o32 movsb
mov eax, esp
sub eax, 8
mov esp, eax
popa
jmp padret

depad:
;repetitions
mov ecx, 4 * 13
;source
mov esi, esp
;destination
mov edi, esp
sub edi, 4 * 13
rep a32 o32 movsb

;repetitions
mov ecx, 4 * 13
;source
mov esi, esp
sub esi, 4 * 13
;destination
mov edi, esp
add edi, 8
rep a32 o32 movsb

mov eax, esp
add eax, 8
mov esp, eax
jmp depadret




%define has_errcode(v) (v == 8 || (v >= 10 && v <= 14) || v == 17 || v == 21)

%assign vector 0
%rep 256
isr%+vector:
%if !has_errcode(vector)
push 0
%endif
push vector
jmp generic_isr
%assign vector vector + 1
%endrep
