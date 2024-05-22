#include "util.h"

void array_copy(char source[], char destination[], int size) {
    for (int i = 0; i < size; i++) {
        destination[i] = source[i];
    }
}

bool array_has_string(char** str_arr, char* the_str) {
    for (size_t i = 0; i < sizeof(str_arr); i++) {
        if (str_arr[i] == the_str) return true;
    }
    return false;
}