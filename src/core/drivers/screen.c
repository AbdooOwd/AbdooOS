#include "screen.h"
#include "../lib/low_level.h"
#include "../lib/mem.h"
#include "../include/types.h"

void print_char(char c, int col, int row) {
    unsigned char* vid = (char*)VIDEO_ADDRESS;
    int offset;

    if (col >= 0 && row >= 0) {
        offset = get_offset(col, row);
    }
    else {
        offset = get_cursor();
    }

    if (c == '\n') {
        int rows = offset / (2 * MAX_COLS);
        offset = get_offset(MAX_COLS, rows);
    }
    else {
        vid[offset] = c;
        offset += 2;
    }

    offset = handle_scrolling(offset);


    set_cursor(offset);
}

/* FUNCITON DOESN'T WORK (prints last char of the string) */
void print_at(char* message, int col, int row) {
    //if (message = 0)
    //    message = "No message provided to print!\n";

    if (col >= 0 && row >= 0) {
        set_cursor(get_offset(col, row));
    }

    int i = 0;
    while (message[i] != 0) {
        print_char(message[i++], col, row);
    }
}

void print(char* message) {
    print_at(message, -1, -1);
}

void print_backspace() {
    int offset = get_cursor() - 2;
    int row = get_offset_row(offset);
    int col = get_offset_col(offset);
    print_char(0, col, row);
    set_cursor(offset);
}

void print_backspaces(int times) {
    for (int i = 0; i < times; i++)
        print_backspace();
}

void set_cursor(int offset) {
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, ((unsigned char)(offset >> 8)) & 0x00FF);
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, offset & 0x00FF);
}

int get_cursor() {
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);

    return offset * 2;
}

int handle_scrolling(int cursor_offset) {

    unsigned char* vidmem = (char*)VIDEO_ADDRESS;

    if (cursor_offset >= MAX_ROWS * MAX_COLS * 2) {
        int i;
        for (i = 1; i < MAX_ROWS; i++)
            memory_copy(get_offset(0, i) + vidmem,
                get_offset(0, i - 1) + vidmem,
                MAX_COLS * 2);
        char* last_line = get_offset(0, MAX_ROWS - 1) + vidmem;
        for (i = 0; i < MAX_COLS * 2; i++)
            if (i % 2 == 0) // so we don't hit the attribute byte (color)
                last_line[i] = 0;
        cursor_offset -= 2 * MAX_COLS;
    }

    return cursor_offset;
}

void clear_screen() {
    int row = 0;
    int col = 0;

    for (row = 0; row < MAX_ROWS; row++) {
        for (col = 0; col < MAX_COLS; col++) {
            print_char(' ', col, row);
        }
    }

    set_cursor(get_offset(0, 0));
}

void color_cell(unsigned char color, int col, int row) {
    unsigned char* vid = (unsigned char*)VIDEO_ADDRESS;

    if (color <= 0)
        color = BLACK_ON_WHITE;

    int offset = get_offset(col, row) / 2;
    vid[offset] = color;
}

int get_offset(int col, int row) { return (row * MAX_COLS + col) * 2; }
int get_offset_row(int offset) { return offset / (2 * MAX_COLS); }
int get_offset_col(int offset) { return (offset - (get_offset_row(offset) * 2 * MAX_COLS)) / 2; }