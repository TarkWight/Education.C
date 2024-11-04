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

#include "game.h"
#include "utils.h"

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
