#include "kernel.h"
#include "../include/stdio.h"
#include "../cpu/isr.h"
#include "../drivers/keyboard.h"
#include "../lib/string.h"

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
    if (str_same(input, "layout")) {
        change_layout();
    }

    if (str_same(input, "test")) {
        //printf("I love %s", "POOP");
    }

    if (str_same(input, "ls")) {
        print("Scanning for files...\nNope! Can't find, I don't have a FAT12 driver (:\n");
    }

    print("$ ");
}