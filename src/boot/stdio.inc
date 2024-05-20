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


bits 32

%define		VIDMEM	0xb8000
%define		COLS	80
%define		LINES	25
%define		CHAR_ATTRIB 15	; white on black

cur_x: db 0					; current x/y location
cur_y: db 0

;	Putch32 ()
;		- Prints a character to screen
;	BL => Character to print

putch32:

	pusha				; save registers
	mov	edi, VIDMEM		; get pointer to video memory

	;-------------------------------;
	;   Get current position	;
	;-------------------------------;

	xor	eax, eax		; clear eax

	;--------------------------------
	; Remember: currentPos = x + y * COLS! x and y are in _CurX and _CurY.
	; Because there are two bytes per character, COLS=number of characters in a line.
	; We have to multiply this by 2 to get number of bytes per line. This is the screen width,
	; so multiply screen with * _CurY to get current line
	;--------------------------------

	mov	ecx, COLS * 2		; Mode 7 has 2 bytes per char, so its COLS*2 bytes per line
	mov	al, byte [cur_y]	; get y pos
	mul	ecx			; multiply y*COLS
	push	eax			; save eax--the multiplication

	;--------------------------------
	; Now y * screen width is in eax. Now, just add cur_x. But, again remember that _CurX is relative
	; to the current character count, not byte count. Because there are two bytes per character, we
	; have to multiply cur_x by 2 first, then add it to our screen width * y.
	;--------------------------------

	mov	al, byte [cur_x]	; multiply _CurX by 2 because it is 2 bytes per char
	mov	cl, 2
	mul	cl
	pop	ecx			; pop y*COLS result
	add	eax, ecx

	;-------------------------------
	; Now eax contains the offset address to draw the character at, so just add it to the base address
	; of video memory (Stored in edi)
	;-------------------------------

	xor	ecx, ecx
	add	edi, eax		; add it to the base address

	;   Watch for new line

	cmp	bl, 0x0A		; is it a newline character?
	je	.row			; yep--go to next row

	;   Print a character 

	mov	dl, bl						; Get character
	mov	dh, CHAR_ATTRIB				; the character attribute
	mov	word [edi], dx				; write to video display

	inc	byte [cur_x]				; go to next character
	cmp	byte [cur_x], COLS		; are we at the end of the line?
	je	.row					; yep-go to next row
	jmp	.done						; nope, bail out

	; next row

.row:
	mov	byte [cur_x], 0		; go back to col 0
	inc	byte [cur_y]		; go to next row

.done:
	mov	bh, byte [cur_y]
	mov	bl, byte [cur_x]
	call move_cursor

	popa				; restore registers and return
	ret



;		- Prints a null terminated string
;	parm\ EBX = address of string to print

puts32:
	pusha
	push	ebx
	pop		edi	; copy string from ebx to edi

	.loop:
		;   Get character

		mov	bl, byte [edi]		; get next character
		cmp	bl, 0				; is it 0 (Null terminator)?
		je	.done				; yep-bail out

		;   Print the character

		call	putch32			; Nope-print it out

		inc	edi			; go to next character
		jmp	.loop

	.done:
		popa				; restore registers, and return
		ret


clear_screen32:
 
	pusha
	cld
	mov	edi, VIDMEM
	mov	cx, 2000
	mov	ah, CHAR_ATTRIB
	mov	al, ' '	
	rep	stosw
 
	mov	byte [cur_x], 0
	mov	byte [cur_y], 0

	mov	bh, byte [cur_y]
	mov	bl, byte [cur_x]
	call	move_cursor
	
	popa
	ret


;		- Update hardware cursor
;	parm/ bh = Y pos
;	parm/ bl = x pos

move_cursor:
 
	pusha				; save registers (aren't you getting tired of this comment?)
 
	;  Get current position
 
	xor	eax, eax
	mov	ecx, COLS
	mov	al, bh			; get y pos
	mul	ecx			; multiply y*COLS
	add	al, bl			; Now add x
	mov	ebx, eax
 
	;--------------------------------------;
	;   Set low byte index to VGA register ;
	;--------------------------------------;
 
	mov	al, 0x0f		; Cursor location low byte index
	mov	dx, 0x03D4		; Write it to the CRT index register
	out	dx, al
 
	mov	al, bl			; The current location is in EBX. BL contains the low byte, BH high byte
	mov	dx, 0x03D5		; Write it to the data register
	out	dx, al			; low byte
 
	;   Set high byte index to VGA register ;
 
	xor	eax, eax
 
	mov	al, 0x0e		; Cursor location high byte index
	mov	dx, 0x03D4		; Write to the CRT index register
	out	dx, al
 
	mov	al, bh			; the current location is in EBX. BL contains low byte, BH high byte
	mov	dx, 0x03D5		; Write it to the data register
	out	dx, al			; high byte
 
	popa
	ret

%endif ;__STDIO_INC_67343546FDCC56AAB872_INCLUDED__