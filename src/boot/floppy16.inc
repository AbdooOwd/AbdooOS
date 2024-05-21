%ifndef __FLOPPY16_ILOVESAUSAGE_INCLUDED__
%define __FLOPPY16_ILOVESAUSAGE_INCLUDED__

bits 16


; OEM Block

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


; data

datasector:     dw 0
cluster:        dw 0

absoluteSector: db 0
absoluteHead:	db 0
absoluteTrack:	db 0


; -------------
;   Functions  
; -------------


cluster_lba:
    sub     ax, 0x0002                          ; zero base cluster number
    xor     cx, cx
    mov     cl, BYTE [bdb_sectors_per_cluster]     ; convert byte to word
    mul     cx
    add     ax, WORD [datasector]               ; base data sector
    ret


; 	LbaChs()
; - Arguments:
;	-	ax: logical sector
;
; Returns absoluteSector, absoluteTrack and absoluteHead

lba_chs:
	xor dx, dx							; clear dx (cuz we'll use dx:ax for operation)
	; absolute sector = (logical sector {ax} / sector per track) + 1
	div WORD [bdb_sectors_per_track]
	inc dl								; dl + 1, now dl has our (logical sector / sector per track)
	mov BYTE [absoluteSector], dl		; store (LogSect / SectPerTrack)

	xor dx, dx 							; clear dx again
	div WORD [bdb_heads_per_cylinder]

	mov BYTE [absoluteHead], dl
	mov BYTE [absoluteTrack], al

	ret


; Reads a series of sectors
; cx: Number of sectors to read
; ax: Starting sector
; es:bx: Buffer to read to
 
read_sectors:
    .MAIN:
        mov     di, 0x0005                          ; five retries for error
    .SECTORLOOP:
        push    ax
        push    bx
        push    cx
        call    lba_chs                             ; convert starting sector to CHS
        mov     ah, 0x02                            ; BIOS read sector
        mov     al, 0x01                            ; read one sector
        mov     ch, BYTE [absoluteTrack]            ; track
        mov     cl, BYTE [absoluteSector]           ; sector
        mov     dh, BYTE [absoluteHead]             ; head
        mov     dl, BYTE [ebr_drive_number]            ; drive
        int     0x13 
        jnc     .SUCCESS                            ; test for read error
        xor     ax, ax                              ; BIOS reset disk
        int     0x13                                ; invoke BIOS
        dec     di                                  ; decrement error counter
        pop     cx
        pop     bx
        pop     ax
        jnz     .SECTORLOOP                         ; attempt to read again
    .SUCCESS:
        pop     cx
        pop     bx
        pop     ax
        add     bx, WORD [bdb_bytes_per_sector]     ; queue next buffer
        inc     ax                                  ; queue next sector
        loop    .MAIN                               ; read next sector
        ret

%endif ;__FLOPPY16_ILOVESAUSAGE_INCLUDED__