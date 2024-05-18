[bits 16]
[org 0x7C00]

%define ENDL 0x0D, 0x0A

; "preparation"?

jmp short start
nop

bdb_oem:                    db 'MSWIN4.1'               ; 8 bytes (recommended by Microsoft)
bdb_bytes_per_sector:       dw 512
bdb_sectors_per_cluster:    db 1
bdb_reserved_sectors:       dw 1
bdb_fat_count:              db 2
bdb_dir_entries_count:      dw 244
bdb_total_sectors:          dw 2880                     ; 2880 * 512 = 1.44MB
bdb_media_descriptor_type:  db 0xf0                     ; F0 = 3.5" floppy disk
bdb_sectors_per_fat:        dw 9                        ; 9 sectors/fat
bdb_sectors_per_track:      dw 18
bdb_heads:                  dw 2
bdb_hidden_sectors:         dd 0
bdb_large_sector_count:     dd 0

; extended boot record
ebr_drive_number:           db 0                        ; 0x00 floppy, 0x80 hdd, useless
                            db 0                        ; reserved
ebr_signature:              db 0x29
ebr_volume_id:              db 0x12, 0x34, 0x56, 0x78   ; serial number, value doesn't matter
ebr_volume_label:           db 'ABDOO    OS'            ; 11 bytes, padded with spaces
ebr_system_id:              db 'FAT12   '               ; 8 bytes


start:

    xor ax, ax
    mov ds, ax
    mov es, ax

    mov si, msg_booting
    call puts

    ; memory size from 0x0 lezgoo
    xor ax, ax
    int 0x12

    cli
    hlt



;
; Functions
;

; Prints text at the cursor's position
;
;   Arguments:
;   -   si: text
puts:
    lodsb
    or  al, al
    jz puts_done
    mov ah, 0x0e        ; print one char
    int 0x10
    jmp puts            ; loop

puts_done:
    ret



.reset:
	mov		ah, 0					; reset floppy disk function
	mov		dl, 0					; drive 0 is floppy drive
	int		0x13					; call BIOS
	jc		.Reset					; If Carry Flag (CF) is set, there was an error. Try resetting again
 
	mov		ax, 0x1000				; we are going to read sector to into address 0x1000:0
	mov		es, ax
	xor		bx, bx

; reads sectors ????
.read:
	mov		ah, 0x02				; function 2
	mov		al, 1					; read 1 sector
	mov		ch, 1					; we are reading the second sector past us, so its still on track 1
	mov		cl, 2					; sector to read (The second sector)
	mov		dh, 0					; head number
	mov		dl, 0					; drive number. Remember Drive 0 is floppy drive.
	int		0x13					; call BIOS - Read the sector
	jc		.Read					; Error, so try again
 
	jmp		0x1000:0x0				; jump to execute the sector!



; labels
msg_loading: db "Loading...", 0
msg_booting: db "Booting AbdooOS...", ENDL, 0


times 510-($-$$) db 0
dw 0xaa55