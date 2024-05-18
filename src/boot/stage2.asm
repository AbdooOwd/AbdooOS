org 0x0
bits 16

%define ENDL 0x0D, 0x0A
 
jmp main


puts:
	lodsb		        ; load next byte from string from SI to AL
	or	al, al	        ; Does AL=0?
    jz	puts_done	    ; Yep, null terminator found-bail out
	mov	ah,	0x0e	    ; Nope-Print the character
	int	0x10
	jmp	puts	        ; Repeat until null terminator found
puts_done:
	ret		            ; we are done, so return


; Second Stage Loader Entry Point
 
main:
    cli		; clear interrupts
	push	cs	; Insure DS=CS
	pop	ds
 
	mov	si, msg
	call	puts
 
	cli		; clear interrupts to prevent triple faults
	hlt		; hault the system


; Data Section
 
msg: db	"Preparing to load operating system...", 13, 10, 0