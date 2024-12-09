#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Константы
const int SIZE = 10;
const char EMPTY = ' ';
const char SHIP = 'S';
const char HIT = 'X';
const char MISS = '.';
const char DESTROYED = 'D';
const int BUFFER_LOCK_CELL = -1;
const int BUFFER_EMPTY_CELL = 0;
const int BUFFER_SHIP_CELL = 1;

// Прототипы функций
int parseCoordinates(char *input, int *x, int *y);
int checkWin(char field[SIZE][SIZE]);

int shoot(char field[SIZE][SIZE], int mask[SIZE][SIZE], int x, int y);
int isShipSunk(char field[SIZE][SIZE], int x, int y);
void placeShipsManual(char field[SIZE][SIZE], int mask[SIZE][SIZE]);
void placeShipsAuto(char field[SIZE][SIZE], int mask[SIZE][SIZE]);
void addBufferZone(int mask[SIZE][SIZE], int x, int y, int length, int horizontal);
int isValidPlacement(int mask[SIZE][SIZE], int x, int y, int length, int horizontal);
void initializeField(char field[SIZE][SIZE], int mask[SIZE][SIZE]);
void printField(char field[SIZE][SIZE], int hideShips);
void computerMoveEasy(char field[SIZE][SIZE], int *x, int *y);
void computerMoveHard(char field[SIZE][SIZE], int *x, int *y);

void printMask(int field[SIZE][SIZE], int isHideShips) {
    printf("   ");
    for (int i = 1; i <= SIZE; i++) printf("%d ", i);
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%c  ", 'A' + i); // A-J for rows
        for (int j = 0; j < SIZE; j++) {
            printf("%d ", field[i][j]);
        }
        printf("\n");
    }
}


int main() {
    char playerField[SIZE][SIZE];
    int playerMask[SIZE][SIZE];

    char computerField[SIZE][SIZE];
    int computerMask[SIZE][SIZE];
    int x, y, playerWin = 0, computerWin = 0;

    srand((unsigned int)time(NULL));

    // Инициализация полей
    initializeField(playerField, playerMask);
    initializeField(computerField, computerMask);

    // Размещение кораблей
    printf("Place your ships:\n");
    placeShipsManual(playerField, playerMask);
    printf("Computer is placing its ships...\n");
    placeShipsAuto(computerField, computerMask);

    // Основной игровой цикл
    while (!playerWin && !computerWin) {
        // Ходы игрока
        printf("Your field:\n");
        printField(playerField, 0);
        printMask(playerMask, 0);
        printf("Computer's field:\n");
        printField(computerField, 1);

        int playerTurn = 1; // Флаг хода игрока
        while (playerTurn && !playerWin) {

            printf("Enter coordinates (e.g., B5): ");
            char input[10];
            scanf("%s", input);

            if (parseCoordinates(input, &x, &y)) {
                if (shoot(computerField, computerMask, x, y)) {
                    printf("Hit! You get another turn.\n");
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
            computerMoveEasy(playerField, &x, &y);
            printf("Computer shoots at (%c%d)\n", 'A' + x, y + 1);
            if (shoot(playerField, playerMask, x, y)) {
                printf("Computer hit your ship! It gets another turn.\n");
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
void placeShipsManual(char field[SIZE][SIZE], int mask[SIZE][SIZE]) {
    int shipSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1}; // Пример кораблей
    int x, y, horizontal;

    for (int i = 0; i < sizeof(shipSizes) / sizeof(shipSizes[0]); i++) {
        int length = shipSizes[i];
        printf("Place a ship of length %d (e.g., B5 1-horizontal, 0-vertical): ", length);
        while (1) {
            char input[10];
            scanf("%s %d", input, &horizontal);
            if (parseCoordinates(input, &x, &y) && isValidPlacement(mask, x, y, length, horizontal)) {
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
void placeShipsAuto(char field[SIZE][SIZE], int mask[SIZE][SIZE]) {
    int shipSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    int x, y, horizontal;

    for (int i = 0; i < sizeof(shipSizes) / sizeof(shipSizes[0]); i++) {
        int length = shipSizes[i];
        while (1) {
            x = rand() % SIZE;
            y = rand() % SIZE;
            horizontal = rand() % 2;
            if (isValidPlacement(mask, x, y, length, horizontal)) {
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
    if (strlen(input) < 2 || input[0] < 'A' || input[0] > 'J' || !isdigit(input[1])) {
        return 0;
    }
    *x = input[0] - 'A';
    *y = atoi(&input[1]) - 1;
    return (*x >= 0 && *x < SIZE && *y >= 0 && *y < SIZE);
}

// Обработка выстрела
int shoot(char field[SIZE][SIZE], int mask[SIZE][SIZE], int x, int y) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)
        return 0;

    if (field[x][y] == SHIP) {
        field[x][y] = HIT;
        return 1;
    } else if (field[x][y] == EMPTY) {
        field[x][y] = MISS;
    }
    return 0;
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
// Ход компьютера
void computerMoveEasy(char field[SIZE][SIZE], int *x, int *y) {
    do {
        *x = rand() % SIZE;
        *y = rand() % SIZE;
    } while (field[*x][*y] == HIT || field[*x][*y] == MISS);
}
