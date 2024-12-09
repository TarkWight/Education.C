#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

const int SIZE = 10;
const char EMPTY = ' ';
const char SHIP = 'S';
const char HIT = 'X';
const char MISS = '.';
const char DESTROYED = 'D';
const int BUFFER_LOCK_CELL = -1;
const int BUFFER_EMPTY_CELL = 0;
const int BUFFER_SHIP_CELL = 1;

typedef struct {
    int x, y;           // Начальная координата
    int size;           // Размер корабля
    int horizontal;     // 1 — горизонтальный, 0 — вертикальный
    int hits;           // Количество попаданий в корабль
} Ship;

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
void huntTarget(AIState* state, char field[SIZE][SIZE], int mask[SIZE][SIZE]);
void executeAITurn(AIState* state, char field[SIZE][SIZE], int mask[SIZE][SIZE]);
void freeAIState(AIState* state);
void computerMove(AIState* aiState, char playerField[SIZE][SIZE],  int playerMask[SIZE][SIZE], int *x, int *y, int aiDifficulty);

void placeShipsManual(char field[SIZE][SIZE], int mask[SIZE][SIZE], Ship ships[], int *shipCount);
void placeShipsAuto(char field[SIZE][SIZE], int mask[SIZE][SIZE], Ship ships[], int *shipCount);
void addBufferZone(int mask[SIZE][SIZE], int x, int y, int length, int horizontal);
void initializeField(char field[SIZE][SIZE], int mask[SIZE][SIZE]);
void printField(char field[SIZE][SIZE], int hideShips);
int isShipSunk(char field[SIZE][SIZE], int x, int y);
int attackCell(char field[SIZE][SIZE], int mask[SIZE][SIZE], int x, int y);
int isValidPlacement(int mask[SIZE][SIZE], int x, int y, int length, int horizontal);
int parseCoordinates(char *input, int *x, int *y);
int checkWin(char field[SIZE][SIZE]);
int shootAndCheckIfShipSunk(char field[SIZE][SIZE], int mask[SIZE][SIZE], Ship ships[], int numShips, int x, int y);

int main() {
    char playerField[SIZE][SIZE];
    int playerMask[SIZE][SIZE];

    char computerField[SIZE][SIZE];
    int computerMask[SIZE][SIZE];
    int x, y, playerWin = 0, computerWin = 0;
    int aiDifficulty;  // Переменная для хранения выбранной сложности ИИ

    srand((unsigned int)time(NULL));

    // Запрос на выбор сложности
    printf("Select AI difficulty (1 - Easy, 2 - Hard): ");
    scanf("%d", &aiDifficulty);

    // Инициализация полей
    initializeField(playerField, playerMask);
    initializeField(computerField, computerMask);

    AIState aiState;
    initializeAIState(&aiState);

    Ship playerShips[10]; // Массив для хранения кораблей игрока
    int numPlayerShips = 0; // Количество кораблей игрока

    Ship computerShips[10]; // Массив для хранения кораблей компьютера
    int numComputerShips = 0;

    // Размещение кораблей
    printf("Place your ships:\n");
    placeShipsManual(playerField, playerMask, playerShips, &numPlayerShips);
    printf("Computer is placing its ships...\n");
    placeShipsAuto(computerField, computerMask, computerShips, &numComputerShips);

    // Основной игровой цикл
    while (!playerWin && !computerWin) {
        // Ходы игрока
        printf("Your field:\n");
        printField(playerField, 0);
        printf("Computer's field:\n");
        printField(computerField, 1);

        int playerTurn = 1; // Флаг хода игрока
        while (playerTurn && !playerWin) {
            printf("Enter coordinates (e.g., B5): ");
            char input[10];
            scanf("%s", input);

            if (parseCoordinates(input, &x, &y)) {
                if (shootAndCheckIfShipSunk(computerField, computerMask, computerShips, numComputerShips, x, y)) {
                    printf("Hit! You get another turn.\n");
                    printField(computerField, 1);
                    if (checkWin(computerField)) {
                        playerWin = 1;
                        break;
                    }
                } else {
                    printf("Miss. Computer's turn.\n");
                    playerTurn = 0; // Передача хода компьютеру
                }
            } else {
                printf("Invalid input. Try again.\n");
            }
        }

        if (playerWin) break;

        // Ходы компьютера
        printf("Computer's turn...\n");
        int computerTurn = 1; // Флаг хода компьютера
        while (computerTurn && !computerWin) {

            printf("Computer shoots at (%c%d)\n", 'A' + y, x + 1);
            computerMove(&aiState, playerField, playerMask, &x, &y, aiDifficulty);  // Вызываем с учетом сложности


            if (shootAndCheckIfShipSunk(playerField, playerMask, playerShips, numPlayerShips, x, y)) {
                printf("Computer hit your ship! It gets another turn.\n");
                printField(playerField, 1);
                if (checkWin(playerField)) {
                    computerWin = 1;
                    break;
                }
            } else {
                printf("Computer missed. Your turn.\n");
                computerTurn = 0; // Передача хода игроку
            }
        }
    }

    // Определение победителя
    if (playerWin) {
        printf("Congratulations, you win!\n");
    } else {
        printf("Computer wins. Better luck next time!\n");
    }

    return 0;
}

// Размещение кораблей вручную
void placeShipsManual(char field[SIZE][SIZE], int mask[SIZE][SIZE], Ship ships[], int *shipCount) {
    int shipSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1}; // Пример кораблей
    int x, y, horizontal;

    for (int i = 0; i < sizeof(shipSizes) / sizeof(shipSizes[0]); i++) {
        int length = shipSizes[i];
        printf("Place a ship of length %d (e.g., B5 1-horizontal, 0-vertical): ", length);
        while (1) {
            char input[10];
            scanf("%s %d", input, &horizontal);
            if (parseCoordinates(input, &x, &y) && isValidPlacement(mask, x, y, length, horizontal)) {
                // Заполнение информации о корабле в структуре
                ships[*shipCount].x = x;
                ships[*shipCount].y = y;
                ships[*shipCount].size = length;
                ships[*shipCount].horizontal = horizontal;
                ships[*shipCount].hits = 0;
                (*shipCount)++; // Увеличиваем счётчик кораблей

                // Размещение корабля на поле
                for (int j = 0; j < length; j++) {
                    if (horizontal)
                        field[x][y + j] = SHIP;
                    else
                        field[x + j][y] = SHIP;
                }

                addBufferZone(mask, x, y, length, horizontal);
                printField(field, 0);
                break;
            } else {
                printf("Invalid placement. Try again: ");
            }
        }
    }
}

// Размещение кораблей компьютером
void placeShipsAuto(char field[SIZE][SIZE], int mask[SIZE][SIZE], Ship ships[], int *shipCount) {
    int shipSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    int x, y, horizontal;

    for (int i = 0; i < sizeof(shipSizes) / sizeof(shipSizes[0]); i++) {
        int length = shipSizes[i];
        while (1) {
            x = rand() % SIZE;
            y = rand() % SIZE;
            horizontal = rand() % 2;
            if (isValidPlacement(mask, x, y, length, horizontal)) {
                // Заполнение информации о корабле в структуре
                ships[*shipCount].x = x;
                ships[*shipCount].y = y;
                ships[*shipCount].size = length;
                ships[*shipCount].horizontal = horizontal;
                ships[*shipCount].hits = 0;
                (*shipCount)++; // Увеличиваем счётчик кораблей

                // Размещение корабля на поле
                for (int j = 0; j < length; j++) {
                    if (horizontal)
                        field[x][y + j] = SHIP;
                    else
                        field[x + j][y] = SHIP;
                }

                addBufferZone(mask, x, y, length, horizontal);
                break;
            }
        }
    }
}

// Проверка корректности размещения корабля
int isValidPlacement(int mask[SIZE][SIZE], int x, int y, int length, int horizontal) {
    for (int i = 0; i < length; i++) {
        int nx = x + (horizontal ? 0 : i);
        int ny = y + (horizontal ? i : 0);
        if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE || mask[nx][ny] != BUFFER_EMPTY_CELL) {
            return 0;
        }
    }
    return 1;
}

// Добавление буферной зоны вокруг корабля
void addBufferZone(int mask[SIZE][SIZE], int x, int y, int length, int horizontal) {
    for (int i = 0; i < length; i++) {
        int nx = x + (horizontal ? 0 : i);
        int ny = y + (horizontal ? i : 0);

        // Помечаем клетки корабля
        mask[nx][ny] = BUFFER_SHIP_CELL;

        // Помечаем клетки вокруг корабля
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int bufferX = nx + dx;
                int bufferY = ny + dy;

                if (bufferX >= 0 && bufferX < SIZE &&
                    bufferY >= 0 && bufferY < SIZE &&
                    mask[bufferX][bufferY] == BUFFER_EMPTY_CELL) {
                    mask[bufferX][bufferY] = BUFFER_LOCK_CELL;
                }
            }
        }
    }
}

// Проверка победы
int checkWin(char field[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (field[i][j] == SHIP)
                return 0;
        }
    }
    return 1;
}

// Парсинг ввода
int parseCoordinates(char *input, int *x, int *y) {
    if (strlen(input) < 2 || (toupper(input[0]) < 'A' || toupper(input[0]) > 'J') || !isdigit(input[1])) {
        return 0;
    }
    *x = toupper(input[0]) - 'A';  // Преобразуем символ в верхний регистр
    *y = atoi(&input[1]) - 1;
    return (*x >= 0 && *x < SIZE && *y >= 0 && *y < SIZE);
}

// Инициализация игрового поля
void initializeField(char field[SIZE][SIZE], int mask[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            field[i][j] = EMPTY;
            mask[i][j] = BUFFER_EMPTY_CELL;
        }
    }
}

// Вывод игрового поля
void printField(char field[SIZE][SIZE], int isHideShips) {
    printf("   ");
    for (int i = 1; i <= SIZE; i++) printf("%d ", i);
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%c  ", 'A' + i); // A-J for rows
        for (int j = 0; j < SIZE; j++) {
            if (isHideShips && field[i][j] == SHIP)
                printf("%c ", EMPTY);
            else
                printf("%c ", field[i][j]);
        }
        printf("\n");
    }
}

void computerMove(AIState* aiState, char playerField[SIZE][SIZE], int playerMask[SIZE][SIZE], int *x, int *y, int aiDifficulty) {
    if (aiDifficulty == 1) {
        // Легкий уровень - случайный выбор клетки
        do {
            *x = rand() % SIZE;
            *y = rand() % SIZE;
        } while (playerField[*x][*y] == HIT || playerField[*x][*y] == MISS); // Не атакованные клетки
    } else {
        // Сложный уровень - использование алгоритма охоты
        executeAITurn(aiState, playerField, playerMask);
        *x = aiState->currentTarget[0];
        *y = aiState->currentTarget[1];
    }
}

// Обработка выстрела и проверка потоплен ли корабль
int shootAndCheckIfShipSunk(char field[SIZE][SIZE], int mask[SIZE][SIZE], Ship ships[], int numShips, int x, int y) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)
        return 0;

    // Обработка попадания
    if (field[x][y] == SHIP) {
        field[x][y] = HIT;

        // Проверка, потоплен ли корабль
        for (int i = 0; i < numShips; i++) {
            Ship *ship = &ships[i]; // Получаем указатель на структуру корабля

            // Проверяем, попал ли выстрел в корабль
            int hit = 0;
            if (ship->horizontal) {
                if (y >= ship->y && y < ship->y + ship->size && x == ship->x) {
                    hit = 1;
                }
            } else {
                if (x >= ship->x && x < ship->x + ship->size && y == ship->y) {
                    hit = 1;
                }
            }

            if (hit) {
                ship->hits++; // Увеличиваем количество попаданий для этого корабля

                // Проверяем, потоплен ли корабль
                if (ship->hits == ship->size) {
                    // Корабль потоплен
                    printf("Sunk a %d-deck ship!\n", ship->size);  // Выводим размер потопленного корабля

                    // Заменяем клетки корабля на DESTROYED
                    for (int j = 0; j < ship->size; j++) {
                        if (ship->horizontal) {
                            field[ship->x][ship->y + j] = DESTROYED; // Горизонтально
                        } else {
                            field[ship->x + j][ship->y] = DESTROYED; // Вертикально
                        }
                    }
                    return 1; // Корабль потоплен
                }
            }
        }
    } else if (field[x][y] == EMPTY) {
        field[x][y] = MISS;
        return 0; // Корабль не потоплен
    }
    return 1;
}

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
