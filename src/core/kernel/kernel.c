#include "kernel.h"
#include "../drivers/screen.h"
#include "../cpu/isr.h"
#include "../drivers/keyboard.h"

char* msg_welcome = "Welcome to AbdooOS 0.2.0";

void initialize_kernel() {
    clear_screen();
    set_cursor(0);

    isr_install();
    irq_install();
    init_keyboard();
}

void kmain_() {

    initialize_kernel();

    print(msg_welcome);
    print("\n\n\n$ ");

    for (;;) {}
}

void user_input(char* input) {
    print(input);
    print("\n");
}