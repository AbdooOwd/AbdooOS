org	0x10000			; Kernel starts at 1 MB
bits	32			; 32 bit code
 
jmp	stage3			; jump to stage 3
 
%include "src/boot/stdio.inc"
 
msg db  0x0A, 0x0A, "Welcome to Kernel Land!!", 0x0A, 0
 
stage3:
 
	;   Set registers
 
	mov	ax, 0x10		    ; set data segments to data selector (0x10)
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 0x90000		; stack begins from 0x90000
 

	;   Clear screen and print success
 
	call 	clear_screen32
	mov		ebx, msg
	call	puts32
 
	;   Stop execution
 
	cli
	hlt