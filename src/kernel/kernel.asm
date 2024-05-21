org	0x100000		; Kernel starts at 1 MB
bits	32			; 32 bit code


jmp	kernel			; jump to kernel

; preprocessor
%include "src/boot/stdio.inc"

%define		COLS	80
%define		ROWS	25
 
; DATA
 
msg_kernel:		db "Kernel Loaded successfully!", 0x0a, 0
msg_welcome: 	
	db 0x0a
	times ( COLS / 4 ) db 0x20	; yeah spaces
	db "Welcome to AbdooOS 0.1.0!", 0x0a, 0

kernel:
 
	;   Set registers
 
	mov	ax, 0x10		    ; set data segments to data selector (0x10 or 16th byte)
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 0x90000		; stack begins from 0x90000
 

	;   Clear screen and print success
 
	call 	clear_screen32

	mov		ebx, msg_kernel
	call	puts32
	
	mov		ebx, msg_welcome
	call 	puts32

	;	I GET MESSAGES FROM THE START!!!
	;	MUST BE FROM ANOTHER GALAXYYY!!!
 
	;   Stop execution
 
	cli
	hlt