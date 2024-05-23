#include "keyboard.h"
#include "../lib/low_level.h"
#include "../cpu/isr.h"
#include "screen.h"
#include "../lib/string.h"
#include "../lib/function.h"
#include "../kernel/kernel.h"
#include "../lib/util.h"


#define BACKSPACE 0x0E
#define ENTER 0x1C
#define LSHIFT 0x2A

#include <stdbool.h>


static char key_buffer[256];

#define SC_MAX 57
char* sc_name_US[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6",
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
        "/", "RShift", "Keypad *", "LAlt", "Spacebar" };

char sc_ascii_US[] = { '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y',
        'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g',
        'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' ' };

char sc_ascii_FR[] = { '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '?', '?', 'a', 'z', 'e', 'r', 't', 'y',
        'u', 'i', 'o', 'p', '[', ']', '?', '?', 'q', 's', 'd', 'f', 'g',
        'h', 'j', 'k', 'l', 'm', '\'', '`', '?', '\\', 'w', 'x', 'c', 'v',
        'b', 'n', ',', ';', '.', '/', '?', '?', '?', ' ' };

char used_layout[SC_MAX + 1];

bool shift_pressed = false;

static void keyboard_callback(registers_t regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    uint8_t scancode = port_byte_in(0x60);

    if (scancode > SC_MAX && !(scancode == 0x2A || scancode == (LSHIFT | 0x80))) return;

    if (scancode == LSHIFT) {
        shift_pressed = true;
        return;
    }
    else if (scancode == (LSHIFT | 0x80)) {
        shift_pressed = false;
        return;
    }

    if (scancode == BACKSPACE) {
        if (strlen(key_buffer) != 0) {
            backspace(key_buffer);
            print_backspace();
        }
    }
    else if (scancode == ENTER) {
        print("\n");
        user_input(key_buffer); /* kernel-controlled function */
        clear_buffer();
    }
    else {
        char letter = used_layout[(int)scancode];

        if (shift_pressed && letter >= 'a' && letter <= 'z')
            letter -= 0x20;

        /* Remember that print only accepts char[] */
        char str[2] = { letter, '\0' };
        append(key_buffer, letter);
        print(str);
    }
    UNUSED(regs);
}

bool isQwerty = false;
void change_layout() {
    isQwerty = !isQwerty;
    if (isQwerty) {
        array_copy(sc_ascii_US, used_layout, SC_MAX + 1);
    }
    else {
        array_copy(sc_ascii_FR, used_layout, SC_MAX + 1);
    }

}

void init_keyboard() {
    array_copy(sc_ascii_FR, used_layout, SC_MAX + 1);
    shift_pressed = false;

    register_interrupt_handler(IRQ1, keyboard_callback);
}

void clear_buffer() {
    key_buffer[0] = '\0';
}