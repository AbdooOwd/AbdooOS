%ifndef _COMMON_INC_INCLUDED
%define _COMMON_INC_INCLUDED

; where the kernel is to be loaded to in protected mode
%define IMAGE_PMODE_BASE 0x1000

; where the kernel is to be loaded to in real mode
%define IMAGE_RMODE_BASE 0x3000

; kernel name (Must be 11 bytes)
image_name:     db "KERNEL  SYS"

; size of kernel image in bytes
image_size:     db 0

%endif