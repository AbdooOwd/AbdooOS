#ifndef STRING_H
#define STRING_H

#include <stdbool.h>
#include "../lib/types.h"

void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);

void reverse(char s[]);
size_t strlen(char s[]);
void backspace(char s[]);
void backspaces(char s[], int times);
void append(char s[], char n);
int strcmp(char s1[], char s2[]);
char* get_first_split(char* str, char split);
void merge_strings(char str1[], char str2[], char result[]);
bool has_char(char* str, char target);
size_t split(char str[], char splitter, char* splitten[]);
int count(char str[], char target);

void lower(char* str);
void upper(char* str);

char* strip_ret(char* str);
void strip(char* to_strip, char* stripped);

#endif