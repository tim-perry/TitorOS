[bits 32]
global open
global write
global read
global clear
global mmap
global fork
global exec
global _wait
global _exit

open:
;takes pointer to string of path on eax
;takes mode on ebx
;returns file descriptor index on eax
mov ebx, [esp + 8]
mov eax, [esp + 4]
push edx
mov edx, 0
int 0x80
pop edx
ret

write:
;takes pointer to buffer on eax
;takes integer fd on ebx
;takes count on ecx
mov ecx, [esp + 12]
mov eax, [esp + 8]
mov ebx, [esp + 4]
push edx
mov edx, 1
int 0x80
pop edx
ret

read:
;takes destination pointer on eax
;tades integer fd on ebx
;takes count on ecx
mov ecx, [esp + 12]
mov eax, [esp + 8]
mov ebx, [esp + 4]
push edx
mov edx, 2
int 0x80
pop edx
ret

clear:
;no arguments
push edx
mov edx, 3
int 0x80
pop edx
ret

mmap:
;takes hint address on eax
;takes length on ebx
;takes flags on ecx
mov ecx, [esp + 12]
mov ebx, [esp + 8]
mov eax, [esp + 4]
push edx
mov edx, 4
int 0x80
pop edx
ret

fork:
;no arguments
push edx
mov edx, 5
int 0x80
pop edx
ret

exec:
;takes pointer to path string of binary to execute on eax
mov eax, [esp + 4]
push edx
mov edx, 6
int 0x80
pop edx
ret

_wait:
;no arguments
mov eax, [esp + 4]
push edx
mov edx, 7
int 0x80
pop edx
ret

_exit: ;until exit syscall is implemented
hlt
jmp _exit