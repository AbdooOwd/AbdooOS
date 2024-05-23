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
bool str_same(char* one, char* two);
void merge_strings(char str1[], char str2[], char result[]);

bool has_char(char* str, char target);
int count(char str[], char target);

void lower(char* str);
void upper(char* str);

#endif