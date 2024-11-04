//===- SticksGame/utils.c - Вспомогательные функции для "Палочек" -*- C -*-===//
//
// Этот файл содержит вспомогательные функции для валидации ввода,
// отображения состояния игры и других утилит.
//
//===----------------------------------------------------------------------===//

#include "utils.h"

// Функция для проверки корректности введённых данных (число или палочки '|')
int is_valid_stick_input(const char* input) {
    // Проверка, если введено число
    if (isdigit(input[0])) {
        int number = atoi(input);
        return number >= 0; // Число палочек должно быть положительным или равно 0
    }

    // Проверка, если введены символы '|'
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] != '|') {
            return 0; // Найден некорректный символ
        }
    }
    return 1; // Все символы являются '|'
}

// Функция для парсинга ввода: количество палочек как число или символы '|'
int parse_input_as_sticks(const char* input) {
    // Если введено число, просто преобразуем его
    if (isdigit(input[0])) {
        return atoi(input);
    }

    // Если введены символы '|', считаем их количество
    int sticks_count = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == '|') {
            sticks_count++;
        }
    }
    return sticks_count;
}

// Функция для получения корректного количества палочек
int get_number_of_sticks() {
    char input[100];
    int num_sticks;

    printf("Enter number of sticks (number or '|') or 0 for a random value (10-20): ");
    scanf("%s", input);

    // Проверка на допустимость введённого значения
    while (!is_valid_stick_input(input)) {
        print_error_message("Invalid input", "Please enter a valid number or '|'.");
        scanf("%s", input);
    }

    // Парсинг введённых данных как количества палочек
    num_sticks = parse_input_as_sticks(input);

    // Если выбрано случайное количество палочек
    if (num_sticks == 0) {
        num_sticks = 10 + rand() % 11;
    }
    return num_sticks;
}

// Функция для валидации целочисленного ввода в пределах диапазона
int get_valid_input(int lower, int upper, const char* prompt, const char* error_message) {
    char input[100];
    int value;
    char *endptr;

    while (1) {
        printf("%s", prompt);
        scanf("%s", input);

        // Попытка конвертации строки в целое число
        value = strtol(input, &endptr, 10);

        // Проверка, что строка полностью сконвертировалась в число и оно в пределах допустимого диапазона
        if (*endptr == '\0' && value >= lower && value <= upper) {
            return value;
        } else {
            print_error_message(error_message, "Please enter a valid number.");
        }
    }
}

// Функция для вывода сообщений об ошибках
void print_error_message(const char* message, const char* details) {
    printf("%s: %s\n", message, details);
}

// Функция отображения палочек
void display_sticks(int n) {
    for (int i = 0; i < n; i++) {
        printf("|");
    }
    printf(" (%d)\n", n);
}
