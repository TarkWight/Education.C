//===- SticksGame/player.c - Логика игроков в "Палочки" -*- C -*-===//
//
// Этот файл содержит функции для управления ходами игроков и
// взаимодействия с ними.
//
//===----------------------------------------------------------------------===//

#include "player.h"
#include "utils.h"

// Функция для получения хода игрока с валидацией
int get_valid_move(int max_sticks) {
    char input[100];
    int move;

    do {
        // Определяем диапазон допустимых значений в зависимости от оставшихся палочек
        if (max_sticks == 1) {
            printf("Enter the number of sticks to remove (1): ");
        } else {
            printf("Enter the number of sticks to remove (1-%d): ", max_sticks < 3 ? max_sticks : 3);
        }

        // Получаем ввод от пользователя
        scanf("%s", input);

        // Проверяем, ввёл ли игрок число или символы палочек
        if (is_valid_stick_input(input)) {
            // Преобразуем символы палочек в число
            move = parse_input_as_sticks(input);
        } else if (isdigit(input[0])) {
            // Преобразуем строку в число, если введено число
            move = atoi(input);
        } else {
            move = 0; // Некорректный ввод
        }

        // Проверка на корректность введённого значения
        if (move < 1 || move > 3 || move > max_sticks) {
            print_error_message("Invalid move", "Please enter a valid number (1-3) and not more than remaining sticks.");
        }
    } while (move < 1 || move > 3 || move > max_sticks);

    return move;
}

// Легкий режим компьютера — случайный ход
int easy_bot_move(int max_sticks) {
    return 1 + rand() % (max_sticks < 3 ? max_sticks : 3);
}

// Сложный режим компьютера — оптимальный ход
int hard_bot_move(int max_sticks) {
    int move = (max_sticks - 1) % 4; // Оставляем кратное 4 количество палочек
    if (move == 0) {
        move = 1 + rand() % (max_sticks < 3 ? max_sticks : 3); // Если нет выигрыша — случайный ход
    }
    return move;
}
