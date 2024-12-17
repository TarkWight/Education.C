#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const int MAX_LINE_LEN = 1000;
const char *RESULT_FILE = "results.txt";
const int MAX_WORDS = 10000;

typedef struct {
    char word[100];
    int frequency;
} WordFrequency;

typedef struct {
    int total_paragraphs;
    int total_sentences;
    int total_words;
    int char_frequency[256];
    WordFrequency *word_frequency;
    int word_count;
} TextStats;

int is_sentence_end(char c) {
    return c == '.' || c == '!' || c == '?';
}

int count_words_in_line(const char *line) {
    int in_word = 0, word_count = 0;
    for (int i = 0; line[i] != '\0'; i++) {
        if (isalnum(line[i])) {
            if (!in_word) {
                in_word = 1;
                word_count++;
            }
        } else {
            in_word = 0;
        }
    }
    return word_count;
}

void count_char_frequency(const char *line, int *char_frequency) {
    for (int i = 0; line[i] != '\0'; i++) {
        unsigned char c = line[i];
        char_frequency[c]++;
    }
}

void add_word_to_frequency(TextStats *stats, const char *word) {
    char word_copy[100];
    strcpy(word_copy, word);

    for (int i = 0; word_copy[i]; i++) {
        if (word_copy[i] == '\'') {
            word_copy[i] = '\0';
            add_word_to_frequency(stats, word_copy);
            add_word_to_frequency(stats, word_copy + i + 1);
            return;
        }
    }

    for (int i = 0; i < stats->word_count; i++) {
        if (strcmp(stats->word_frequency[i].word, word_copy) == 0) {
            stats->word_frequency[i].frequency++;
            return;
        }
    }

    strcpy(stats->word_frequency[stats->word_count].word, word_copy);
    stats->word_frequency[stats->word_count].frequency = 1;
    stats->word_count++;
}

int compare_words(const void *a, const void *b) {
    return strcmp(((WordFrequency *)a)->word, ((WordFrequency *)b)->word);
}

void load_previous_results(TextStats *stats) {
    FILE *file = fopen(RESULT_FILE, "r");
    if (!file) {
        return;
    }

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Paragraphs:", 10) == 0) {
            sscanf(line, "Paragraphs: %d", &stats->total_paragraphs);
        } else if (strncmp(line, "Sentences:", 9) == 0) {
            sscanf(line, "Sentences: %d", &stats->total_sentences);
        } else if (strncmp(line, "Words:", 6) == 0) {
            sscanf(line, "Words: %d", &stats->total_words);
        } else if (strncmp(line, "Character Frequency", 19) == 0) {
            // Загружаем частоту символов
            while (fgets(line, sizeof(line), file) && line[0] != '=' && line[0] != '\n') {
                unsigned char c;
                int frequency;
                if (line[0] == '\\' && line[1] == 'n') { // Обрабатываем символ новой строки
                    sscanf(line, "\\n: %d", &frequency);
                    stats->char_frequency['\n'] += frequency;
                } else if (sscanf(line, "%c: %d", &c, &frequency) == 2) {
                    stats->char_frequency[c] += frequency;
                }
            }
        } else if (strncmp(line, "Word Frequency", 14) == 0) {
            while (fgets(line, sizeof(line), file) && line[0] != '=' && line[0] != '\n') {
                char word[100];
                double relative_frequency;
                if (sscanf(line, "%s: %lf", word, &relative_frequency) == 2) {
                    int absolute_frequency = (int)(relative_frequency * stats->total_words);
                    int word_found = 0;

                    for (int i = 0; i < stats->word_count; i++) {
                        if (strcmp(stats->word_frequency[i].word, word) == 0) {
                            stats->word_frequency[i].frequency += absolute_frequency;
                            word_found = 1;
                            break;
                        }
                    }

                    if (!word_found) {
                        strcpy(stats->word_frequency[stats->word_count].word, word);
                        stats->word_frequency[stats->word_count].frequency = absolute_frequency;
                        stats->word_count++;
                    }
                }
            }
        }
    }

    fclose(file);

    // // Напечатаем все слова в строку
    // printf("\nLoaded Words:\n");
    // for (int i = 0; i < stats->word_count; i++) {
    //     printf("%s ", stats->word_frequency[i].word);
    // }
    // printf("\n");

    // // Напечатаем все символы в строку
    // printf("\nLoaded Character Frequencies:\n");
    // for (int i = 0; i < 256; i++) {
    //     if (stats->char_frequency[i] > 0) {
    //         if (i == '\n') {
    //             printf("\\n ");
    //         } else {
    //             printf("%c ", i);
    //         }
    //     }
    // }
    // printf("\n");
}



void analyze_text(const char *filename, TextStats *stats) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char line[MAX_LINE_LEN];
    int prev_line_empty = 1;
    stats->total_paragraphs = 0;
    stats->total_sentences = 0;
    stats->total_words = 0;

    load_previous_results(stats);

    while (fgets(line, sizeof(line), file)) {
        int line_len = strlen(line);

        if (line_len == 1 && line[0] == '\n') {
            prev_line_empty = 1;
            continue;
        }

        if (prev_line_empty) {
            stats->total_paragraphs++;
        }
        prev_line_empty = 0;
        int line_words = count_words_in_line(line);
        stats->total_words += line_words;

        char word[100];
        int word_index = 0;

        for (int i = 0; i < line_len; i++) {
            if (isalnum(line[i])) {
                word[word_index++] = tolower(line[i]);
            } else {
                if (word_index > 0) {
                    word[word_index] = '\0';
                    add_word_to_frequency(stats, word);
                    word_index = 0;
                }
            }
            if (is_sentence_end(line[i])) {
                stats->total_sentences++;
            }
        }

        if (word_index > 0) {
            word[word_index] = '\0';
            add_word_to_frequency(stats, word);
        }

        count_char_frequency(line, stats->char_frequency);
    }

    fclose(file);
}


void save_results(const TextStats *stats) {
    FILE *file = fopen(RESULT_FILE, "w");
    if (!file) {
        printf("Error opening file: %s\n", RESULT_FILE);
        return;
    }

    fprintf(file, "===========================\n");
    fprintf(file, "General Text Statistics\n");
    fprintf(file, "===========================\n");

    fprintf(file, "Paragraphs: %d\n", stats->total_paragraphs);
    fprintf(file, "Sentences: %d\n", stats->total_sentences);
    fprintf(file, "Words: %d\n", stats->total_words);
    fprintf(file, "Average words per sentence: %.2f\n\n",
            (stats->total_sentences > 0) ? (double)stats->total_words / stats->total_sentences : 0.0);

    fprintf(file, "===========================\n");
    fprintf(file, "Character Frequency\n");
    fprintf(file, "===========================\n");

    for (int i = 0; i < 256; i++) {
        if (stats->char_frequency[i] > 0) {
            if (i == '\n') {
                fprintf(file, "\\n: %d\n", stats->char_frequency[i]);
            } else {
                fprintf(file, "%c: %d\n", i, stats->char_frequency[i]);
            }
        }
    }

    fprintf(file, "\n===========================\n");
    fprintf(file, "Word Frequency\n");
    fprintf(file, "===========================\n");

    qsort(stats->word_frequency, stats->word_count, sizeof(WordFrequency), compare_words);

    for (int i = 0; i < stats->word_count; i++) {
        fprintf(file, "%s: %.2f\n", stats->word_frequency[i].word,
                (double)stats->word_frequency[i].frequency / stats->total_words);
    }

    fclose(file);
}


void search_word(TextStats *stats, char *word) {
    for (int i = 0; word[i]; i++) {
        word[i] = tolower(word[i]);
    }

    int low = 0, high = stats->word_count - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int cmp = strcmp(stats->word_frequency[mid].word, word);
        // 1 если символ в первой строке больше символа во второй
        // -1 если этот первый различающийя символ меньше, чем в первой строке
        // 0 если нет различающихся символов
        if (cmp == 0) {
            printf("Word '%s' found.\n", word);
            return;
        }
        if (cmp < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    printf("Word '%s' not found.\n", word);
}

int main() {
    const char *input_file = "input.txt";
    TextStats stats = {0, 0, 0, {0}, NULL, 0};

    stats.word_frequency = malloc(MAX_WORDS * sizeof(WordFrequency));
    if (!stats.word_frequency) {
        printf("Memory allocation failed\n");
        return 1;
    }

    load_previous_results(&stats);

    char choice;
    do {
        printf("\nChoose an option:\n");
        printf("s/S - Search for a word\n");
        printf("a/A - Analyze a new file\n");
        printf("c/C - Exit\n");
        printf("Enter your choice: ");
        scanf(" %c", &choice);

        if (choice == 's' || choice == 'S') {
            char search_word_str[100];
            printf("Enter word to search: ");
            scanf("%s", search_word_str);
            search_word(&stats, search_word_str);
        }
        else if (choice == 'a' || choice == 'A') {
            printf("Analyzing file: %s\n", input_file);
            analyze_text(input_file, &stats);

            save_results(&stats);
            printf("Results saved to: %s\n", RESULT_FILE);
        }
        else if (choice == 'c' || choice == 'C') {
            printf("Exiting program.\n");
            break;
        }
        else {
            printf("Invalid choice. Please try again.\n");
        }

    } while (choice != 'c' && choice != 'C');

    free(stats.word_frequency);
    return 0;
}



