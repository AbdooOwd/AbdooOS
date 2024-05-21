%ifndef __GDT_INC_67343546FDCC56AAB872_INCLUDED__
%define __GDT_INC_67343546FDCC56AAB872_INCLUDED__
 
bits 16
 
; InstallGDT()
;	- Install our GDT
 
install_GDT:
 
	cli				    ; clear interrupts
	pusha				; save registers
	lgdt 	[toc]		; load GDT into GDTR
	sti				    ; enable interrupts
	popa				; restore registers
	ret				    ; All done!
 
; Global Descriptor Table (GDT)
 
; 8 bytes long
gdt_data: 
	dd 0 				; null descriptor
	dd 0 				

;	We stored 2 double words, which is: 
;		size = 2 bytes * 2 double words = 2 bytes * (2 * double word) = 8 bytes!!!!
 
; kernel space code:	; code descriptor
	dw 0x0FFFF			; limit low
	dw 0 				; base low
	db 0 				; base middle
	db 10011010b 		; access
	db 11001111b 		; granularity
	db 0 				; base high
 
; kernel space data:	; data descriptor
	dw 0x0FFFF			; limit low (Same as code)
	dw 0 				; base low
	db 0 				; base middle
	db 10010010b 		; access
	db 11001111b 		; granularity
	db 0				; base high

; user space code (offset: 24 (0x18))
	dw 0x0FFFF			; limit low
	dw 0				; base low
	db 0				; base middle
	db 11111010b		; access (bit 5 and 6 for privilege (ring 3))
	db 11001111b		; granularity (???)
	db 0				; base high

; user space data (offset 32)
	dw 0x0FFFF
	dw 0
	dw 0
	db 11110010b
	db 11001111b
	db 0

	

end_of_gdt:
toc: 
	dw end_of_gdt - gdt_data - 1 	; limit (Size of GDT)
	dd gdt_data 			; base of GDT

%define NULL_DESC 0
%define CODE_DESC 0x8
%define DATA_DESC 0x10

%endif ;__GDT_INC_67343546FDCC56AAB872_INCLUDED__