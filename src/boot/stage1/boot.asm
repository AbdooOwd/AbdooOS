[bits 16]
[org 0x7c00]


_start:
    mov si, msg_loading
    call puts

; prints whats in bx
puts:
    mov al, [si]
	mov ah, 0x0e
    cmp al, 0
    jz puts_done
    int 0x10
    inc si
    jmp puts

puts_done:
    ret


; labels
msg_loading: db "Hello World", 0


times 510-($-$$) db 0
dw 0xaa55