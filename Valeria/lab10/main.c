#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STR_LEN 50
#define NUM_CLOTHES 3

typedef struct {
    int uid;
    char name[MAX_STR_LEN];
    float weight;
    char category[MAX_STR_LEN];
    int year;
    double price;
    char color[MAX_STR_LEN];
} Fruit;

void getRandomStringFromFile(const char *filename, char *result) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Cannot open file");
        exit(1);
    }

    char buffer[MAX_STR_LEN];
    int line_count = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        line_count++;
    }


    int random_line = rand() % line_count;

    rewind(file);
    for (int i = 0; i <= random_line; i++) {
        fgets(result, MAX_STR_LEN, file);
    }

    result[strcspn(result, "\n")] = 0;

    fclose(file);
}

void generateFruits(Fruit *fruit) {
    for (int i = 0; i < NUM_CLOTHES; i++) {
        getRandomStringFromFile("names.txt", fruit[i].name);
        fruit[i].uid = rand() % 1000;
        fruit[i].weight = (float)(rand() % 1000) / 10;
        getRandomStringFromFile("categories.txt", fruit[i].category);
        fruit[i].year = 2000 + rand() % 24;
        fruit[i].price = (double)(rand() % 100000) / 100;
        getRandomStringFromFile("colors.txt", fruit[i].color);
    }
}

void displayFruit(const Fruit *fruit) {
    for (int i = 0; i < NUM_CLOTHES; i++) {
        printf("Name: %s\n", fruit[i].name);
        printf("UID: %d\n", fruit[i].uid);
        printf("Weight: %.2f kg\n", fruit[i].weight);
        printf("Category: %s\n", fruit[i].category);
        printf("Year: %d\n", fruit[i].year);
        printf("Price: %.2lf USD\n", fruit[i].price);
        printf("Color: %s\n", fruit[i].color);
        printf("-------------------------\n");
    }
}

int main() {
    srand(time(NULL));
    Fruit fruit[NUM_CLOTHES];

    generateFruits(fruit);

    displayFruit(fruit);

    return 0;
}
