%ifndef __STDIO_INC_67343546FDCC56AAB872_INCLUDED__
%define __STDIO_INC_67343546FDCC56AAB872_INCLUDED__

;	Puts16 ()
;		-Prints a null terminated string
;	DS=>SI: 0 terminated string
 
bits 16
 
puts16:
		pusha				; save registers
.loop1:
		lodsb				; load next byte from string from SI to AL
		or	al, al			; Does AL=0?
		jz	puts16done		; Yep, null terminator found-bail out
		mov	ah, 0eh			; Nope-Print the character
		int	10h			    ; invoke BIOS
		jmp	.loop1			; Repeat until null terminator found
puts16done:
		popa				; restore registers
		ret				    ; we are done, so return
 
%endif ;__STDIO_INC_67343546FDCC56AAB872_INCLUDED__