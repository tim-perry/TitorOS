bits 16

hang:
    hlt
    jmp hang


print:
    ;string pointer on es:bx
    mov ah, 0x0e ;teletype output

    printloop:
    mov al, [bx]
    cmp al, 0
    je exitprint
    int 0x10
    inc bx
    jmp printloop

    exitprint:
    ret


read:
    ;drive on dl    
    ;CHS on cx
    ;number of sectors on ax
    ;destination on es:bx
    pusha
    push ax
    mov ah, 0x2 ;read mode
    mov dh, 0 ;head
    clc
    int 0x13
    jc disk_error   ;carry bit set is error
    cmp ah, 0       ;al != 0 is error
    jne disk_error
    pop bx
    cmp al, bl      ;number of read sectors on al, check matches with al input
    jne sect_error
    popa
    ret

    sect_error:
    mov bx, sect_error_msg
    call print

    disk_error:
    mov bx, disk_error_msg
    call print

    jmp hang


;error messages
sect_error_msg:
    db "Incorrect number of sectors read", 0x0a, 0x0d, 0
disk_error_msg:
    db "Error reading disk", 0x0a, 0x0d, 0