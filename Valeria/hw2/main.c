#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Константы
const int SIZE = 10; // Размер игрового поля
const char EMPTY = ' '; // Пустая клетка
const char SHIP = 'S'; // Корабль
const char HIT = 'X'; // Попадание
const char MISS = '.'; // Промах

// Прототипы функций
void initializeField(char field[SIZE][SIZE]);
void printField(char field[SIZE][SIZE], int hideShips);
void placeShipsManual(char field[SIZE][SIZE]);
void placeShipsAuto(char field[SIZE][SIZE]);
int isValidPlacement(char field[SIZE][SIZE], int x, int y, int length, int horizontal);
int shoot(char field[SIZE][SIZE], int x, int y);
void computerMove(char field[SIZE][SIZE], int *x, int *y);
int checkWin(char field[SIZE][SIZE]);

int main() {
    char playerField[SIZE][SIZE], computerField[SIZE][SIZE];
    int x, y, playerWin = 0, computerWin = 0;

    srand((unsigned int)time(NULL));

    // Инициализация полей
    initializeField(playerField);
    initializeField(computerField);

    // Размещение кораблей
    printf("Place your ships:\n");
    placeShipsManual(playerField);
    printf("Computer is placing its ships...\n");
    placeShipsAuto(computerField);

    // Основной игровой цикл
    while (!playerWin && !computerWin) {
        // Ход игрока
        printf("Your field:\n");
        printField(playerField, 0);
        printf("Computer's field:\n");
        printField(computerField, 1);

        printf("Enter your shot coordinates (x y): ");
        scanf("%d %d", &x, &y);
        if (shoot(computerField, x, y)) {
            printf("Hit!\n");
        } else {
            printf("Miss.\n");
        }
        if (checkWin(computerField)) {
            playerWin = 1;
            break;
        }

        // Ход компьютера
        printf("Computer's turn...\n");
        computerMove(playerField, &x, &y);
        printf("Computer shoots at (%d, %d)\n", x, y);
        if (shoot(playerField, x, y)) {
            printf("Computer hit your ship!\n");
        } else {
            printf("Computer missed.\n");
        }
        if (checkWin(playerField)) {
            computerWin = 1;
            break;
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

// Инициализация игрового поля
void initializeField(char field[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            field[i][j] = EMPTY;
        }
    }
}

// Вывод игрового поля
void printField(char field[SIZE][SIZE], int hideShips) {
    printf("  ");
    for (int i = 0; i < SIZE; i++) printf("%d ", i);
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", i);
        for (int j = 0; j < SIZE; j++) {
            if (hideShips && field[i][j] == SHIP)
                printf("%c ", EMPTY);
            else
                printf("%c ", field[i][j]);
        }
        printf("\n");
    }
}

// Размещение кораблей вручную
void placeShipsManual(char field[SIZE][SIZE]) {
    int shipSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1}; // Пример кораблей
    int x, y, horizontal;

    for (int i = 0; i < sizeof(shipSizes) / sizeof(shipSizes[0]); i++) {
        int length = shipSizes[i];
        printf("Place a ship of length %d (x y horizontal[1-horizontal, 0-vertical]): ", length);
        while (1) {
            scanf("%d %d %d", &x, &y, &horizontal);
            if (isValidPlacement(field, x, y, length, horizontal)) {
                for (int j = 0; j < length; j++) {
                    if (horizontal)
                        field[x][y + j] = SHIP;
                    else
                        field[x + j][y] = SHIP;
                }
                break;
            } else {
                printf("Invalid placement. Try again: ");
            }
        }
    }
}

// Размещение кораблей компьютером
void placeShipsAuto(char field[SIZE][SIZE]) {
    int shipSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    int x, y, horizontal;

    for (int i = 0; i < sizeof(shipSizes) / sizeof(shipSizes[0]); i++) {
        int length = shipSizes[i];
        while (1) {
            x = rand() % SIZE;
            y = rand() % SIZE;
            horizontal = rand() % 2;
            if (isValidPlacement(field, x, y, length, horizontal)) {
                for (int j = 0; j < length; j++) {
                    if (horizontal)
                        field[x][y + j] = SHIP;
                    else
                        field[x + j][y] = SHIP;
                }
                break;
            }
        }
    }
}

// Проверка корректности размещения корабля
int isValidPlacement(char field[SIZE][SIZE], int x, int y, int length, int horizontal) {
    for (int i = 0; i < length; i++) {
        int nx = x + (horizontal ? 0 : i);
        int ny = y + (horizontal ? i : 0);
        if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE || field[nx][ny] != EMPTY)
            return 0;
    }
    return 1;
}

// Обработка выстрела
int shoot(char field[SIZE][SIZE], int x, int y) {
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

// Ход компьютера
void computerMove(char field[SIZE][SIZE], int *x, int *y) {
    do {
        *x = rand() % SIZE;
        *y = rand() % SIZE;
    } while (field[*x][*y] == HIT || field[*x][*y] == MISS);
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


