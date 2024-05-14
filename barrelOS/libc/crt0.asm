[bits 32]
[extern init_libc]
[extern main]
[extern exit]

global _start

_start:
;starts out with stack set with whatever exec() put on it



;init io streams and heap for malloc
;shoudl exec() arguments be able to decide io streams? (envp???)
call init_libc

call main ;return value on eax
call exit ;takes exit code on eax