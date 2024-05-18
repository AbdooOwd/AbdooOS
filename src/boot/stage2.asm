bits	16

; Remember the memory map-- 0x500 through 0x7bff is unused above the BIOS data area.
; We are loaded at 0x500 (0x50:0)

org 0x500

jmp	main				; go to start

;	Preprocessor directives

%include "src/boot/stdio.inc"
%include "src/boot/gdt.inc"
%define ENDL 0x0D, 0x0A

;	Data Section

msg_loading: db "Preparing to load operating system...", ENDL, 0x00

;	STAGE 2 ENTRY POINT
;
;		-Store BIOS information
;		-Load Kernel
;		-Install GDT; go into protected mode (pmode)
;		-Jump to Stage 3

main:

	;-------------------------------;
	;   Setup segments and stack	;
	;-------------------------------;

	cli				; clear interrupts
	xor	ax, ax			; null segments
	mov	ds, ax
	mov	es, ax
	mov	ax, 0x9000		; stack begins at 0x9000-0xffff
	mov	ss, ax
	mov	sp, 0xFFFF
	sti				; enable interrupts

	;-------------------------------;
	;   Print loading message	;
	;-------------------------------;

	mov	si, msg_loading
	call	puts16

	;-------------------------------;
	;   Install our GDT
	;-------------------------------;

	call	install_GDT		; install our GDT

	;-------------------------------;
	;   Go into pmode		;
	;-------------------------------;


    ; enable A20
    mov ax, 0x2401
    int 0x15

	cli				; clear interrupts
	mov	eax, cr0		; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax

	jmp	0x08:Stage3		; far jump to fix CS. Remember that the code selector is 0x8!

;	ENTRY POINT FOR STAGE 3

bits 32					; Welcome to the 32 bit world!

Stage3:

	;-------------------------------;
	;   Set registers		;
	;-------------------------------;

	mov		ax, 0x10		; set data segments to data selector (0x10)
	mov		ds, ax
	mov		ss, ax
	mov		es, ax
	mov		esp, 0x90000		; stack begins from 90000h

;	Stop execution

STOP:

	cli
	hlt