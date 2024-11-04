//===- StickGame/game.h - Заголовочный файл для игры "Палочки" -*- C -*-===//

#ifndef GAME_H
#define GAME_H

void game_loop(int num_sticks, int player_mode, int difficulty);
int easy_bot_move(int max_sticks);
int hard_bot_move(int max_sticks);
int get_valid_move(int max_sticks);

#endif // GAME_H
