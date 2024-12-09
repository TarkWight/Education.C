//===- SticksGame/game.c - Логика игры "Палочки" -*- C -*-===//
//
// Этот файл содержит логику для игры "Палочки", включая управление
// игровым процессом и основным игровым циклом.
//
//===----------------------------------------------------------------------===//

#include "game.h"
#include "utils.h"

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
