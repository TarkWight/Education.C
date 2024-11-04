//===- SticksGame/main.c - Игра "Палочки" -*- C -*-===//
//
// Данная программа реализует игру "Палочки", в которой два игрока
// по очереди снимают палочки со стола. Игра продолжается до тех
// пор, пока не останется палочек, и проигрывает тот, кто снимет
// последнюю палочку. Программа поддерживает как одиночный режим,
// где один игрок противостоит компьютеру, так и многопользовательский
// режим, где играют два человека.
//
//===----------------------------------------------------------------------===//
//
/// \file
/// Этот файл содержит логику для игры "Палочки", включая получение
/// вводных данных от пользователя, отображение текущего состояния
/// игры и управление игровым процессом.
///
/// \section Input
/// Программа запрашивает у пользователя следующую информацию:
/// - В количестве палочек (N), которые будут на столе.
/// - Режим игры (одиночный или многопользовательский).
/// - Уровень сложности для компьютерного противника (если выбран одиночный режим).
/// - Количество палочек, которое игрок желает снять в каждой итерации.
///
/// \section Output
/// Программа выводит текущее состояние игры, включая количество
/// оставшихся палочек, а также их графическое представление в виде
/// символов " | ". После завершения игры программа предлагает
/// начать новую игру.
///
/// \section Example
///
/// **Одиночный режим:**
/// Ввод:
/// 10
/// 1
/// 2
///
/// Вывод:
/// Текущие палочки: |||||||||| (10)
/// Ход игрока 1: Сколько палочек снять (1-3)?
/// 2
/// Текущие палочки: ||||||| (8)
/// Компьютер снимает 1 палочку.
/// Текущие палочки: |||||| (7)
/// ...
/// Игрок 1 проигрывает!
///
/// **Многопользовательский режим:**
/// Ввод:
/// 15
/// 2
///
/// Вывод:
/// Текущие палочки: ||||||||||||||||| (15)
/// Ход игрока 1: Сколько палочек снять (1-3)?
/// 3
/// Текущие палочки: ||||||||||||||| (12)
/// Ход игрока 2: Сколько палочек снять (1-3)?
/// ...
/// Игрок 2 проигрывает!
///
//===----------------------------------------------------------------------===//
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


void display_sticks(int n); // Done
int get_valid_move(int max_sticks);  // Done
int easy_bot_move(int max_sticks);
int hard_bot_move(int max_sticks);
void game_loop(int num_sticks, int player_mode, int difficulty); // Done
int parse_input_as_sticks(const char* input);
int is_valid_stick_input(const char* input);
int get_valid_input(int lower, int upper, const char* prompt, const char* error_message);  // Done
void print_error_message(const char* message, const char* details); // Done
int get_number_of_sticks();

int main() {
    int num_sticks, player_mode, difficulty;
    char play_again;

    // Инициализация генератора случайных чисел
    srand(time(NULL));

    do {
        // Выбор режима игры (один или два игрока)
        player_mode = get_valid_input(1, 2, "Choose game mode:\n1. Single player (against computer)\n2. Two players\n", "Invalid game mode.");

        if (player_mode == 1) {
            // Выбор уровня сложности
            difficulty = get_valid_input(1, 2, "Choose difficulty:\n1. Easy\n2. Hard\n", "Invalid difficulty level.");
        }

        // Ввод количества палочек
        num_sticks = get_number_of_sticks();

        // Запуск основной игровой логики
        game_loop(num_sticks, player_mode, difficulty);

        // Спрашиваем игрока о новой игре
        printf("Do you want to play again? (y/n): ");
        scanf(" %c", &play_again);

    } while (play_again == 'y' || play_again == 'Y');

    return 0;
}


// Основной игровой цикл
void game_loop(int num_sticks, int player_mode, int difficulty) {
    int current_player = 1;
    int move;

    while (num_sticks > 0) {
        printf("\nCurrent sticks: ");
        display_sticks(num_sticks);

        if (player_mode == 2 || (player_mode == 1 && current_player == 1)) {
            // Ход игрока
            printf("Player %d's turn:\n", current_player);
            move = get_valid_move(num_sticks);
        } else {
            // Ход компьютера
            if (difficulty == 1) {
                move = easy_bot_move(num_sticks); // Легкий режим
            } else {
                move = hard_bot_move(num_sticks); // Сложный режим
            }
            printf("Computer removes %d sticks.\n", move);
        }

        num_sticks -= move;

        // Проверка на окончание игры
        if (num_sticks == 0) {
            printf("\nPlayer %d loses!\n", current_player);
            return;
        }

        // Смена игрока
        current_player = (current_player == 1) ? 2 : 1;
    }
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
