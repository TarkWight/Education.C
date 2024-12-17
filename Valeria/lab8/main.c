#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LEN 50
#define MAX_WORDS 1000
#define MAX_TEXT_LEN 10000

// Структура для хранения слова и его частоты
typedef struct {
    char word[MAX_WORD_LEN];
    int count;
} WordInfo;

typedef struct {
    int paragraph;
    int sentence;
    int word_index;
} WordPosition;

WordInfo words[MAX_WORDS];
int word_count = 0;

int is_separator(char c) {
    return isspace(c) || c == '.' || c == ',' || c == '-' || c == '\"' || c == '\'' || c == '\n';
}

void add_word(char *word) {
    if (strlen(word) == 0) return;

    for (int i = 0; i < word_count; i++) {
        if (strcmp(words[i].word, word) == 0) {
            words[i].count++;
            return;
        }
    }
    if (word_count < MAX_WORDS) {
        strcpy(words[word_count].word, word);
        words[word_count].count = 1;
        word_count++;
    }
}

void analyze_text(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char c, word[MAX_WORD_LEN] = {0};
    int word_index = 0, total_words = 0;

    while ((c = fgetc(file)) != EOF) {
        if (is_separator(c)) {
            if (word_index > 0) {
                word[word_index] = '\0';
                add_word(word);
                total_words++;
                word_index = 0;
            }
        } else {
            if (word_index < MAX_WORD_LEN - 1) {
                word[word_index++] = tolower(c);
            }
        }
    }

    fclose(file);
    printf("Analyzed %d words.\n", total_words);
}

void save_results(const char *result_file) {
    FILE *file = fopen(result_file, "w");
    if (!file) {
        printf("Error opening file: %s\n", result_file);
        return;
    }

    for (int i = 0; i < word_count; i++) {
        fprintf(file, "%s %d\n", words[i].word, words[i].count);
    }

    fclose(file);
    printf("Results saved to %s\n", result_file);
}

int main() {
    const char *input_file = "input.txt";
    const char *output_file = "results.txt";

    analyze_text(input_file);
    save_results(output_file);

    return 0;
}
