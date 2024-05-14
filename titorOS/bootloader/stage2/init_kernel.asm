;gdt segments
extern gdt_descriptor
extern CODE32_SEG
extern DATA32_SEG
extern CODE64_SEG
extern DATA64_SEG

;externs
extern boot

bits 32
global init_kernel
init_kernel:

;load pml4
mov eax, [esp + 4]
mov cr3, eax

;Enable PAE
mov eax, cr4
or eax, (1 << 5)
mov cr4, eax

;set long mode enable
mov ecx, 0xc0000080
rdmsr
or eax, (1 << 8)
wrmsr

;enable paging
mov eax, cr0
or eax, (1 << 31)
mov cr0, eax

;switch to 64 bit gdt segment
jmp CODE64_SEG:initlong
bits 64
initlong:
mov eax, DATA64_SEG
mov ds, eax
mov ss, eax
mov es, eax
mov fs, eax
mov gs, eax
mov rbp, 0xffffffff80080000
mov rsp, rbp

;jmp to kernel
mov rdi, boot
mov rax, hang
push rax
push 0xffffffff80000000
ret

hang:
hlt
jmp hang