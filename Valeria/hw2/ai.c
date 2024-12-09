#include "ai.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void initializeAIState(AIState* state) {
    state->mask = malloc(SIZE * sizeof(int*));
    for (int i = 0; i < SIZE; i++) {
        state->mask[i] = malloc(SIZE * sizeof(int));
    }

    state->hitQueue = malloc(SIZE * SIZE * sizeof(int[2]));
    state->hitQueueSize = 0;
    state->huntingMode = 0;
    state->currentTarget[0] = -1;
    state->currentTarget[1] = -1;
}

void enqueueAdjacentCells(AIState* state, int x, int y) {
    const int directions[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };

    for (int i = 0; i < 4; i++) {
        int nx = x + directions[i][0];
        int ny = y + directions[i][1];

        if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE) {
            if (state->mask[nx][ny] == 0) {
                state->mask[nx][ny] = 1; // Помечаем как "ожидаемая цель"
                state->hitQueue[state->hitQueueSize][0] = nx;
                state->hitQueue[state->hitQueueSize][1] = ny;
                state->hitQueueSize++;
            }
        }
    }
}

void handleShipDestroyed(AIState* state, int x, int y, int length, int horizontal) {
    for (int i = 0; i < length; i++) {
        int nx = x + (horizontal ? 0 : i);
        int ny = y + (horizontal ? i : 0);

        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int bufferX = nx + dx;
                int bufferY = ny + dy;

                if (bufferX >= 0 && bufferX < SIZE && bufferY >= 0 && bufferY < SIZE) {
                    state->mask[bufferX][bufferY] = -1; // Убираем из поиска
                }
            }
        }
    }
    state->huntingMode = 0; // Завершаем охоту
}

int attackCell(char field[SIZE][SIZE], int mask[SIZE][SIZE], int x, int y) {
    if (field[x][y] == SHIP) {
        field[x][y] = HIT;
        mask[x][y] = HIT;
        return 1; // Попадание
    }
    field[x][y] = MISS;
    mask[x][y] = MISS;
    return 0; // Промах
}

void huntTarget(AIState* state, char field[SIZE][SIZE], int mask[SIZE][SIZE]) {
    while (state->hitQueueSize > 0) {
        int x = state->hitQueue[0][0];
        int y = state->hitQueue[0][1];

        for (int i = 1; i < state->hitQueueSize; i++) {
            state->hitQueue[i - 1][0] = state->hitQueue[i][0];
            state->hitQueue[i - 1][1] = state->hitQueue[i][1];
        }
        state->hitQueueSize--;

        if (state->mask[x][y] == 0) {
            int hit = attackCell(field, mask, x, y);
            if (hit) {
                enqueueAdjacentCells(state, x, y);
            } else {
                state->mask[x][y] = -1; // Промах
            }
            return;
        }
    }
}

void executeAITurn(AIState* state, char field[SIZE][SIZE], int mask[SIZE][SIZE]) {
    if (state->huntingMode) {
        huntTarget(state, field, mask); // Охота на корабль
    } else {
        int x, y;
        do {
            x = rand() % SIZE;
            y = rand() % SIZE;
        } while (state->mask[x][y] != 0); // Ищем неатакованную ячейку

        int hit = attackCell(field, mask, x, y);
        if (hit) {
            state->huntingMode = 1;
            state->currentTarget[0] = x;
            state->currentTarget[1] = y;
            enqueueAdjacentCells(state, x, y); // Добавляем соседние ячейки в очередь для поиска
        } else {
            state->mask[x][y] = -1; // Промах
        }
    }
}

void freeAIState(AIState* state) {
    for (int i = 0; i < SIZE; i++) {
        free(state->mask[i]);
    }
    free(state->mask);
    free(state->hitQueue);
}
