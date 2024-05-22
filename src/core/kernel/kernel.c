// #include "kernel.h"
// #include "../drivers/screen.h"

void kmain_() {
    // print_char('E', MAX_COLS - 3, MAX_ROWS - 3);
    unsigned char* vid = 0xb8000;
    *vid = 'Q';

    for (;;) {}
}