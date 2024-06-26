bits	16

; Remember the memory map-- 0x500 through 0x7bff is unused above the BIOS data area.
; We are loaded at 0x500 (0x50:0)

org 0x500

jmp	main				; go to start



;	Preprocessor directives

%include    "src/boot/stdio.inc"
%include    "src/boot/gdt.inc"
%include    "src/boot/A20.inc"
%include	"src/boot/floppy16.inc"
%include	"src/boot/fat12.inc"
%include	"src/boot/common.inc"

%define     ENDL 	0x0D, 0x0A
%define		VIDMEM	0xB8000

;	Data Section

msg_loading:    db "Preparing to load operating system...", ENDL, 0
msg_gdt:        db "Installing GDT...", 					ENDL, 0
msg_a20:        db "Enabling A20, 20th address line...", 	ENDL, 0
msg_pmode:      db "Entering Protected Mode...", 			ENDL, 0
msg_failure:		db "ERROR OCCURED!",						ENDL, 0

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

	;   Print loading message	;

	mov	si, msg_loading
	call	puts16

	;   Install our GDT
    mov si, msg_gdt
    call puts16

	call	install_GDT

    ; enable A20
    mov si, msg_a20
    call puts16
    
    call	EnableA20_KKbrd_Out


	; Load Root Directory
	call load_root

	; load kernel
	mov		ebx, 0					; BX:BP points to buffer to load to
    mov		bp, IMAGE_RMODE_BASE
	mov		si, image_name			; our file to load
	call	load_file				; load our file
	mov		dword [image_size], ecx	; save size of kernel
	cmp		ax, 0					; Test for success
	je		enter_stage3			; yep--onto Stage 3!
	mov		si, msg_failure			; Nope--print error
	call	puts16
	mov		ah, 0
	int     0x16                    ; await keypress
	int     0x19                    ; warm boot computer
	cli								; If we get here, something really went wong
	hlt



enter_stage3:

	mov si, msg_pmode
    call puts16

	cli				; clear interrupts
	mov	eax, cr0		; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax

	jmp	CODE_DESC:stage3	; far jump to fix CS. Remember that the code selector is 0x8!

	; Note: Do NOT re-enable interrupts! Doing so will triple fault!
	; We will fix this in Stage 3.

;	ENTRY POINT FOR STAGE 3

bits 32					; Welcome to the 32 bit world!


; 32 bit data
msg_registers:	db "Setting up segment registers and the stack...", 0x0a, 0
msg_copying:	db "Copying kernel to 0x100000 (1Mb)...", 0x0a, 0

; 0x8???

stage3:

	;-------------------------------;
	;   Set registers		;
	;-------------------------------;

	mov ebx, msg_registers
	call puts32

	mov	ax, DATA_DESC	; set data segments to data selector (0x10)
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 0x90000	; stack begins from 90000h


	; Copy kernel to 1MB
	

copy_image:

	mov ebx, msg_copying
	call puts32

  	mov	eax, dword [image_size]
  	movzx	ebx, word [bdb_bytes_per_sector]
  	mul	ebx
  	mov	ebx, 4
  	div	ebx
   	cld
   	mov    esi, IMAGE_RMODE_BASE
   	mov	edi, IMAGE_PMODE_BASE
   	mov	ecx, eax
   	rep	movsd                   ; copy image to its protected mode address


	;   Execute Kernel

	jmp	CODE_DESC:IMAGE_PMODE_BASE; jump to our kernel! Note: This assumes Kernel's entry point is at 1 MB


	;   Stop execution

	cli
	hlt