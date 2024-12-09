//===- SticksGame/utils.h - Заголовочный файл для утилит "Палочки" -*- C -*-===//

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>

int is_valid_stick_input(const char* input);
int parse_input_as_sticks(const char* input);
int get_number_of_sticks();
int get_valid_input(int lower, int upper, const char* prompt, const char* error_message);
void print_error_message(const char* message, const char* details);
void display_sticks(int n);

#endif // UTILS_H
