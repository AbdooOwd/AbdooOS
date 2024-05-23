#include "string.h"
#include "../drivers/screen.h"
#include "util.h"
#include <stdbool.h>

/**
 * K&R implementation
 */
void int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

void hex_to_ascii(int n, char str[]) {
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    int32_t tmp;
    int i;
    for (i = 28; i > 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) continue;
        zeros = 1;
        if (tmp > 0xA) append(str, tmp - 0xA + 'a');
        else append(str, tmp + '0');
    }

    tmp = n & 0xF;
    if (tmp >= 0xA) append(str, tmp - 0xA + 'a');
    else append(str, tmp + '0');
}

/* K&R */
void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* K&R */
size_t strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0') ++i;
    return i;
}

void append(char s[], char n) {
    int len = strlen(s);
    s[len] = n;
    s[len + 1] = '\0';
}

void backspace(char s[]) {
    int len = strlen(s);
    s[len - 1] = '\0';
}

void backspaces(char s[], int times) {
    for (int i = 0; i < times; i++) {
        backspace(s);
    }
}


/* K&R
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */
int strcmp(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}

bool str_same(char* one, char* two) {
    if (strcmp(one, two) == 0) {
        return true;
    }
    
    return false;
}


void merge_strings(char str1[], char str2[], char result[]) {
    // TODO: Fix that it adds a space between strings
    array_copy(str1, result, strlen(str1));
    for (int i = 0; i < strlen(str2); i++)
        append(result, str2[i]);
}

bool has_char(char* str, char target) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == target) return true;
    }
    return false;
}

int count(char str[], char target) {
    int le_count = 0;
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == target) le_count++;
    }
    return le_count;
}

void lower(char* str) {
    for (int i = 0; i < strlen(str); i++)
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] += 0x20;
}

void upper(char* str) {
    for (int i = 0; i < strlen(str); i++)
        if (str[i] >= 'a' && str[i] <= 'z')
            str[i] -= 0x20;
}