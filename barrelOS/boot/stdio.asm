putchar: ;takes character on al
pusha
mov ah, 0x0e
int 0x10
popa
ret

getchar: ;returns ascii code on al
mov ah, 0x00
int 0x16
mov ah, 0x00 ;clear scancode on ah (so ascii value can be used easily
ret

print: ;takes pointer to null-terminating string on ax
pusha
mov bx, ax
mov ah, 0x0e
printloop:
mov al, [bx]
cmp al, 0
je exitprint
int 0x10
inc bx
jmp printloop
exitprint:
popa
ret

;printhex: ;takes hex value on ax
;pusha
;mov bx, ax ;create copy of hex value
;mov al, "0"
;call putchar
;mov al, "x"
;call putchar
;mov cl, 16 ;amount of shift
;printhexloop:
;sub cl, 4 ;cl now loaded with amount to shift by this time
;jl exithex ;if cl < 0, then done
;mov ax, bx ;refresh hex value
;shr ax, cl ;shift ax right
;and ax, 0x000F ;isolate last 4 bits
;add al, 48 ;add 48 regardless
;cmp al, 57 ;check if greater than ascii "9"
;jle notletter
;add al, 7 ;add another 7 if ABCDEF
;notletter:
;call putchar
;jmp printhexloop
;exithex:
;popa
;ret

;printdec: ;takes decimal value on ax (limited to be less than 2560)
;pusha
;mov cl, 0 ;counter
;mov dl, 10
;convert:
;div dl ;quotient in al, remainder in ah
;push ax ;push remainder to stack (in ah)
;inc cl ;keep track of how many digits are in the stack
;cmp al, 0 ;check if quotient is 0
;mov ah, 0
;jne convert ;if not, loop
;printdecloop:
;pop ax
;mov al, ah ;remainder needs to be in lower half
;add al, 48 ;ascii value
;call putchar
;dec cl
;jne printdecloop
;popa
;ret
