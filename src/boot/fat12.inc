%ifndef __FAT12_GALAXY_INCLUDED__
%define __FAT12_GALAXY_INCLUDED__

; > Preprocessor stuff

; constants
%define ROOT_OFFSET 0x2e00
%define FAT_SEG 0x2c0
%define ROOT_SEG 0x2e0

; includes

%include "src/boot/floppy16.inc"


; -------------
;   Functions
; -------------


;       load_root
; - Loads Root Directory

load_root:
	pusha						            ; store registers
	push	es


    ;
    ;   Calculating size of root directory (in sectors)
    ;

    xor     cx, cx					        ; clear registers
 	xor     dx, dx
	mov     ax, 32					        ; 32 byte directory entry
	mul     WORD [bdb_dir_entries_count]	; total size of directory
	div     WORD [bdb_bytes_per_sector]		; sectors used by directory
	xchg    ax, cx					        ; move into AX

    ; now AX has the number of sectors in the root directory


    ;
    ;   Compute location of RootDir and store it in AX
    ;

	mov     al, byte [bdb_fat_count]		; number of FATs
	mul     word [bdb_sectors_per_fat]		; sectors used by FATs
	add     ax, word [bdb_reserved_sectors]	; adjust for bootsector
	mov     word [datasector], ax			; base of root directory
	add     word [datasector], cx

    ;
    ;   Read RootDir
    ;

    push	word ROOT_SEG
	pop	es
	mov     bx, 0x0					        ; copy root dir
	call    read_sectors				    ; read in directory table
	pop	es
	popa						            ; restore registers and return
	ret


loadFAT:
	pusha				                    ; store registers
	push	es

    ;
    ;   Compute size of FAT and store it in CX
    ;

	xor     ax, ax
	mov     al, BYTE [bdb_fat_count]		; number of FATs
	mul     word [bdb_sectors_per_fat]		; sectors used by FATs
	mov     cx, ax

    ;
    ; Compute location of FAT and store it in AX
    ;

	mov     ax, word [bdb_reserved_sectors]

    ;
    ; read FAT into memory (Overwrite our bootloader at 0x7c00)
    ;

	push	word FAT_SEG
	pop	es
	xor	bx, bx
	call  read_sectors
	pop	es
	popa		                            ; restore registers and return
	ret


; find_file ()
;	- Search for filename in root table
;
; parm  - ds:si:    File name
; ret   - ax:       File index number in directory table. -1 if error

find_file:
    push    cx
    push    dx
    push    bx
    mov     bx, si                          ; copy filename for later

    ; browse RootDir for Binary Image

    mov cx, WORD [bdb_dir_entries_count]    ; load loop counter
    mov di, ROOT_OFFSET                     ; locate first Root Entry
    cld                                     ; clear direction flag

    .loop:
        push cx
        mov cx, 11                          ; eleven chars, image name is in SI
        mov si, bx                      ; image name now in bx
        push di
        rep cmpsb                    ; test for entry match

        pop di
        je .found
        pop cx
        add di, 32              ; queue next direction entry
        loop .loop

    .not_found:
        pop bx
        pop dx
        pop cx
        mov ax, -1
        ret

    .found:
        pop ax                  ; return value into AX contains entry of file
        pop bx                  ; resotre registers and return
        pop cx
        pop dx
        ret


; load_file ()
;	- Load file
; parm - es:si :    File to load
; parm - bx:bp :    Buffer to load file to
; ret - ax :        -1 on error, 0 on success
; ret - cx :        Number of sectors loaded

load_file:

    ; we'll use ecx to keep track of readen sectors count

    xor ecx, ecx                ; clear ecx
    push ecx

.FIND_FILE:

	push	bx	            ; BX=>BP points to buffer to write to; store it for later
	push	bp

	call	find_file		; find our file. ES:SI contains our filename

    ; ES:DI contains location of the first root entry

	cmp	ax, -1				; check for error
	jne	.load_image_pre		; No error :) Load the FAT
	pop	bp				    ; Nope :( Restore registers, set error code and return
	pop	bx
	pop	ecx
	mov	ax, -1
	ret
    
.load_image_pre:
 
	sub	edi, ROOT_OFFSET
	sub	eax, ROOT_OFFSET
 
	; get starting cluster
 
	push	word ROOT_SEG
	pop	es
	mov     dx, word [es:di + 0x001A]   ; ES:DI points to file entry in root directory table.
	mov     word [cluster], dx	        ; Refrence the table for file's first cluster
	pop	    bx			                ; get location to write to so we dont screw up the stack
	pop	    es
	push    bx			                ; store location for later again
	push	es

    call loadFAT ; load FAT, duh
    
.load_image:
 
	mov     ax, WORD [cluster]		        ; cluster to read
	pop	    es
	pop     bx
	call    cluster_lba			            ; convert cluster to LBA
 	xor     cx, cx
	mov     cl, BYTE [bdb_sectors_per_cluster]	; sectors to read
	call    read_sectors			            ; Read in cluster
 
	pop	    ecx				                    ; increment sector count
	inc	    ecx
	push	ecx
 
	push    bx				                ; save registers for next iteration
	push    es
 
	mov	    ax, FAT_SEG
	mov	    es, ax
	xor	    bx, bx

    ; compute next cluster
 
	mov     ax, WORD [cluster]	; identify current cluster
	mov     cx, ax			; copy current cluster
	mov     dx, ax			; copy current cluster
	shr     dx, 0x0001		; divide by two
	add     cx, dx			; sum for (3/2)
 
	mov     bx, 0			; location of FAT in memory
	add     bx, cx			; index into FAT
	mov     dx, WORD [es:bx]	; read two bytes from FAT
	test    ax, 0x0001
	jnz     .ODD_CLUSTER
 
.EVEN_CLUSTER:
	and     dx, 0000111111111111b	; take low twelve bits
	jmp     .DONE
 
.ODD_CLUSTER:
	shr     dx, 0x0004		; take high twelve bits
          
.DONE:
	mov     WORD [cluster], dx	; store new cluster
	cmp     dx, 0x0FF0		; test for end of file marker (0xFF)
	jb      .load_image		; No? Go on to next cluster then
 
.SUCCESS:
	pop	es			; restore all registers
	pop	bx
	pop     ecx
	xor     ax, ax			; return success code
	ret
    


%endif ;__FAT12_GALAXY_INCLUDED__