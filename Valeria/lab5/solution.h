#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdio.h>
/*
 *
 * Функции, которые нужно реализовать.
 * Здесь используются шаблоны функций.
 * Реализация находится ниже.
 *
*/
int* reverse_negatives(const int* arr, int size);
int* reverse_odd(const int* arr, int size);
int* reverse_non_zero(const int* arr, int size);
int* reverse_positive(const int* arr, int size);
int* separate_odd(const int* arr, int size);

/*
 *
 * Вспомогательные функции.
 * Для работы вышеописанных функций обязательна allocate_memory
 * так как упрощает выделение памяти для массивов, сокращая код
 * Заполнение массива - чисто опциональная функция, нужная
 * для тестирования вышеописанного кода
 * solution - упрощённый вызов всего этого добра.
 *
*/
void print_array(const int* arr, int size);
void* allocate_memory(size_t size);
void fill_array(int* arr, int size, int left_range, int right_range);
void solution(const int* array, int size);

#endif // SOLUTION_H
