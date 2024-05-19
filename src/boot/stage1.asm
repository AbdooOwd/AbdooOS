
[bits 16]	; we are in 16 bit real mode
[org 0]		; we will set regisers later

%define ENDL 0x0D, 0x0A

start:	jmp	main					; jump to start of bootloader


;	BIOS Parameter Block

bdb_oem:                    db 'ABDOOOS3'               ; 8 bytes (recommended by Microsoft)
bdb_bytes_per_sector:       dw 512
bdb_sectors_per_cluster:    db 1
bdb_reserved_sectors:       dw 1
bdb_fat_count:              db 2
bdb_dir_entries_count:      dw 244
bdb_total_sectors:          dw 2880                     ; 2880 * 512 = 1.44MB
bdb_media_descriptor_type:  db 0xf0                     ; F0 = 3.5" floppy disk
bdb_sectors_per_fat:        dw 9                        ; 9 sectors/fat
bdb_sectors_per_track:      dw 18
bdb_heads_per_cylinder:     dw 2
bdb_hidden_sectors:         dd 0
bdb_large_sector_count:     dd 0

; extended boot record
ebr_drive_number:           db 0                        ; 0x00 floppy, 0x80 hdd, useless
                            db 0                        ; reserved
ebr_signature:              db 0x29
ebr_volume_id:              db 0x12, 0x34, 0x56, 0x78   ; serial number, value doesn't matter
ebr_volume_label:           db 'ABDOO    OS'            ; 11 bytes, padded with spaces
ebr_system_id:              db 'FAT12   '               ; 8 bytes

;************************************************;
;	Prints a string
;	DS=>SI: 0 terminated string
;************************************************;
print:
			lodsb				; load next byte from string from SI to AL
			or	al, al			; Does AL=0?
			jz	print_done		; Yep, null terminator found-bail out
			mov	ah, 0eh			; Nope-print the character
			int	10h
			jmp	print			; Repeat until null terminator found
	print_done:
			ret				; we are done, so return

;************************************************;
; Reads a series of sectors
; CX=>Number of sectors to read
; AX=>Starting sector
; ES:BX=>Buffer to read to
;************************************************;

read_sectors:
    .MAIN:
        mov     di, 0x0005                          ; five retries for error
    .SECTORLOOP:
        push    ax
        push    bx
        push    cx
        call    LBACHS                              ; convert starting sector to CHS
        mov     ah, 0x02                            ; BIOS read sector
        mov     al, 0x01                            ; read one sector
        mov     ch, BYTE [absoluteTrack]            ; track
        mov     cl, BYTE [absoluteSector]           ; sector
        mov     dh, BYTE [absoluteHead]             ; head
        mov     dl, BYTE [ebr_drive_number]            ; drive
        int     0x13                                ; invoke BIOS
        jnc     .SUCCESS                            ; test for read error
        xor     ax, ax                              ; BIOS reset disk
        int     0x13                                ; invoke BIOS
        dec     di                                  ; decrement error counter
        pop     cx
        pop     bx
        pop     ax
        jnz     .SECTORLOOP                         ; attempt to read again
        int     0x18
    .SUCCESS:
        mov     si, msgProgress
        call    print
        pop     cx
        pop     bx
        pop     ax
        add     bx, WORD [bdb_bytes_per_sector]        ; queue next buffer
        inc     ax                                  ; queue next sector
        loop    .MAIN                               ; read next sector
        ret



; Convert CHS to LBA
; LBA = (cluster - 2) * sectors per cluster

ClusterLBA:
    sub     ax, 0x0002                          ; zero base cluster number
    xor     cx, cx
    mov     cl, BYTE [bdb_sectors_per_cluster]     ; convert byte to word
    mul     cx
    add     ax, WORD [datasector]               ; base data sector
    ret



; Convert LBA to CHS
; AX=>LBA Address to convert
;
; absolute sector = (logical sector / sectors per track) + 1
; absolute head   = (logical sector / sectors per track) MOD number of heads
; absolute track  = logical sector / (sectors per track * number of heads)
;


LBACHS:
          xor     dx, dx                              ; prepare dx:ax for operation
          div     WORD [bdb_sectors_per_track]           ; calculate
          inc     dl                                  ; adjust for sector 0
          mov     BYTE [absoluteSector], dl
          xor     dx, dx                              ; prepare dx:ax for operation
          div     WORD [bdb_heads_per_cylinder]          ; calculate
          mov     BYTE [absoluteHead], dl
          mov     BYTE [absoluteTrack], al
          ret



;	Bootloader Entry Point


main:

     ; code located at 0000:7C00, adjust segment registers

        cli								; disable interrupts
        mov     ax, 0x07C0				; setup registers to point to our segment
        mov     ds, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax

     ; create stack

        mov     ax, 0x0000				; set the stack
        mov     ss, ax
        mov     sp, 0xFFFF
        sti								; restore interrupts

    ; Display loading message

        mov     si, msgLoading
        call    print

    ; Load root directory table


    load_root:

    ; compute size of root directory and store in "cx"

        xor     cx, cx
        xor     dx, dx
        mov     ax, 0x0020                           ; 32 byte directory entry
        mul     WORD [bdb_dir_entries_count]         ; total size of directory
        div     WORD [bdb_bytes_per_sector]          ; sectors used by directory
        xchg    ax, cx

     ; compute location of root directory and store in "ax"

        mov     al, BYTE [bdb_fat_count]            ; number of FATs
        mul     WORD [bdb_sectors_per_fat]          ; sectors used by FATs
        add     ax, WORD [bdb_reserved_sectors]     ; adjust for bootsector
        mov     WORD [datasector], ax               ; base of root directory
        add     WORD [datasector], cx

     ; read root directory into memory (7C00:0200)

        mov     bx, 0x0200                          ; copy root dir above bootcode
        call    read_sectors

     ;----------------------------------------------------
     ; Find stage 2
     ;----------------------------------------------------

     ; browse root directory for binary image
        mov     cx, WORD [bdb_dir_entries_count]             ; load loop counter
        mov     di, 0x0200                            ; locate first root entry
    .LOOP:
        push    cx
        mov     cx, 0x000B                            ; eleven character name
        mov     si, ImageName                         ; image name to find
        push    di
    rep  cmpsb                                         ; test for entry match
        pop     di
        je      load_fat
        pop     cx
        add     di, 0x0020                            ; queue next directory entry
        loop    .LOOP
        jmp     FAILURE

     ;----------------------------------------------------
     ; Load FAT
     ;----------------------------------------------------

    load_fat:

     ; save starting cluster of boot image

        mov     si, msgCRLF
        call    print
        mov     dx, WORD [di + 0x001A]
        mov     WORD [cluster], dx                  ; file's first cluster

     ; compute size of FAT and store in "cx"

        xor     ax, ax
        mov     al, BYTE [bdb_fat_count]          ; number of FATs
        mul     WORD [bdb_sectors_per_fat]             ; sectors used by FATs
        mov     cx, ax

     ; compute location of FAT and store in "ax"

        mov     ax, WORD [bdb_reserved_sectors]       ; adjust for bootsector

     ; read FAT into memory (7C00:0200)

        mov     bx, 0x0200                          ; copy FAT above bootcode
        call    read_sectors

     ; read image file into memory (0050:0000)

        mov     si, msgCRLF
        call    print
        mov     ax, 0x0050
        mov     es, ax                              ; destination for image
        mov     bx, 0x0000                          ; destination for image
        push    bx

     ;----------------------------------------------------
     ; Load Stage 2
     ;----------------------------------------------------

    load_image:

        mov     ax, WORD [cluster]                  ; cluster to read
        pop     bx                                  ; buffer to read into
        call    ClusterLBA                          ; convert cluster to LBA
        xor     cx, cx
        mov     cl, BYTE [bdb_sectors_per_cluster]     ; sectors to read
        call    read_sectors
        push    bx

     ; compute next cluster

        mov     ax, WORD [cluster]                  ; identify current cluster
        mov     cx, ax                              ; copy current cluster
        mov     dx, ax                              ; copy current cluster
        shr     dx, 0x0001                          ; divide by two
        add     cx, dx                              ; sum for (3/2)
        mov     bx, 0x0200                          ; location of FAT in memory
        add     bx, cx                              ; index into FAT
        mov     dx, WORD [bx]                       ; read two bytes from FAT
        test    ax, 0x0001
        jnz     .ODD_CLUSTER

    .EVEN_CLUSTER:

        and     dx, 0000111111111111b               ; take low twelve bits
        jmp     .DONE

    .ODD_CLUSTER:

        shr     dx, 0x0004                          ; take high twelve bits

    .DONE:

        mov     WORD [cluster], dx                  ; store new cluster
        cmp     dx, 0x0FF0                          ; test for end of file
        jb      load_image

    DONE:

        mov     si, msgCRLF
        call    print
        push    WORD 0x0050
        push    WORD 0x0000
        retf

    FAILURE:

        mov     si, msgFailure
        call    print
        mov     ah, 0x00
        int     0x16                                ; await keypress
        int     0x19                                ; warm boot computer

    absoluteSector: db 0x00
    absoluteHead:   db 0x00
    absoluteTrack:  db 0x00

    datasector:  dw 0x0000
    cluster:     dw 0x0000
    ImageName:   db "STAGE2  SYS"
    msgLoading:  db ENDL, "Loading Boot Image...", ENDL, 0x00
    msgCRLF:     db ENDL, 0x00
    msgProgress: db ".", 0x00
    msgFailure:  db ENDL, "ERROR : Press Any Key to Reboot", ENDL, 0x00

          TIMES 510-($-$$) DB 0
          DW 0xAA55
