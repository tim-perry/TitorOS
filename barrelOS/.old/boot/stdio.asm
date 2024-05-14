putchar:;pass char on al
	pusha
	mov ah,0x0e
	int 0x10
	popa
	ret

getchar:;return char on ax (ascii code on al)
	mov ah,0x00 ;read keypress
	int 0x16
	mov ah,0x00 ;clear scancode on ah
	ret

print:;pass array on ax
	pusha
	mov bx,ax
	mov ah,0x0e
	
	printloop:
	mov al,[bx]
	cmp al,0
	je printend
	int 0x10
	inc bx
	jmp printloop

	printend:
	popa
	ret

printhex:;pass 16bit hex number on ax
	push cx
	push ax
	push ax
	push ax
	push ax
	push ax
	mov al,'0'
	call putchar
	mov al, 'x'
	call putchar
	mov ch,4	;count characters to print

	printhexloop:
	pop ax		;refresh hex

	mov cl,ch	;times to shift=chars to print
	shift:		;cl can be an operand of shr, better way to implement?
	dec cl
	je convert
	shr ax,4
	jmp shift
	
	convert:
	and ax, 0x000F	;isolate last digit
	add ax,0x30	;ad 30h regardless
	cmp ax,0x39	;check if letter or not
	jle notletter
	add ax,0x07
	notletter:
	call putchar
	dec ch
	jne printhexloop

	pop ax
	pop cx
	ret

printdec:;pass 16bit number on ax, destroys ax because of recursion, cant pop
	mov dx,0
	mov cx,0x0A
	div cx
	push dx
	cmp ax,0
	je printdigit
	call printdec
	
	printdigit:
	pop ax
	add al,'0'
	call putchar
	ret
	
