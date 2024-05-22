#include "kernel.h"
#include "../drivers/screen.h"

char* msg_welcome = "Welcome to AbdooOS 0.2.0";

void initialize_kernel() {
    clear_screen();
    set_cursor(0);
}

void kmain_() {

    initialize_kernel();

    print(msg_welcome);
    print("\n\n\n$ ");

    for (;;) {}
}