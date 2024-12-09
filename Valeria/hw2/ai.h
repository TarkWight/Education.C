#ifndef AI_H
#define AI_H

#include "constants.h"

typedef struct {
    int** mask;          // Состояние ячеек: не атакована, ожидаемая, атакована
    int** hitQueue;      // Очередь для локального поиска
    int hitQueueSize;    // Размер очереди
    int huntingMode;     // Режим: 0 - поиск, 1 - охота
    int currentTarget[2]; // Координаты первого попадания
} AIState;


void initializeAIState(AIState* state);
void enqueueAdjacentCells(AIState* state, int x, int y);
void handleShipDestroyed(AIState* state, int x, int y, int length, int horizontal);
int attackCell(char field[SIZE][SIZE], int mask[SIZE][SIZE], int x, int y);
void huntTarget(AIState* state, char field[SIZE][SIZE], int mask[SIZE][SIZE]);
void executeAITurn(AIState* state, char field[SIZE][SIZE], int mask[SIZE][SIZE]);
void freeAIState(AIState* state);

#endif // AI_H
