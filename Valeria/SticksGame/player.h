//===- SticksGame/player.h - Заголовочный файл для игроков "Палочки" -*- C -*-===//

#ifndef PLAYER_H
#define PLAYER_H

int get_valid_move(int max_sticks);
int easy_bot_move(int max_sticks);
int hard_bot_move(int max_sticks);

#endif // PLAYER_H
