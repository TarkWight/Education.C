#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const int MAX_LINE_LEN = 1000;
const char *RESULT_FILE = "results.txt";

typedef struct {
    int total_paragraphs;
    int total_sentences;
    int total_words;
    int char_frequency[256];
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

void analyze_text(const char *filename, TextStats *stats) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char line[MAX_LINE_LEN];
    int sentence_ended = 1;
    stats->total_paragraphs = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strcmp(line, "\n") == 0) {
            stats->total_paragraphs++;
            continue;
        }

        int line_words = count_words_in_line(line);
        stats->total_words += line_words;

        for (int i = 0; line[i] != '\0'; i++) {
            if (is_sentence_end(line[i])) {
                stats->total_sentences++;
                sentence_ended = 1;
            } else if (!isspace(line[i])) {
                sentence_ended = 0;
            }
        }
        if (!sentence_ended) stats->total_sentences++;

        count_char_frequency(line, stats->char_frequency);
    }

    fclose(file);
    if (stats->total_paragraphs == 0) stats->total_paragraphs = 1;
}

void load_previous_results(TextStats *stats) {
    FILE *file = fopen(RESULT_FILE, "r");
    if (!file) return;

    fscanf(file, "Paragraphs: %d\n", &stats->total_paragraphs);
    fscanf(file, "Sentences: %d\n", &stats->total_sentences);
    fscanf(file, "Words: %d\n", &stats->total_words);

    for (int i = 0; i < 256; i++) {
        fscanf(file, "%d", &stats->char_frequency[i]);
    }

    fclose(file);
}

void save_results(const TextStats *stats) {
    FILE *file = fopen(RESULT_FILE, "w");
    if (!file) {
        printf("Error opening file: %s\n", RESULT_FILE);
        return;
    }

    fprintf(file, "Paragraphs: %d\n", stats->total_paragraphs);
    fprintf(file, "Sentences: %d\n", stats->total_sentences);
    fprintf(file, "Words: %d\n", stats->total_words);
    fprintf(file, "Average words per sentence: %.2f\n",
            (double)stats->total_words / stats->total_sentences);

    for (int i = 0; i < 256; i++) {
        if (stats->char_frequency[i] > 0) {
            if (i == '\n') {
                fprintf(file, "\\n: %d\n", stats->char_frequency[i]);
            } else {
                fprintf(file, "%c: %d\n", i, stats->char_frequency[i]);
            }
        }
    }

    fclose(file);
}

int main() {
    const char *input_file = "input.txt";
    TextStats stats = {0, 0, 0, {0}};

    load_previous_results(&stats);

    printf("Analyzing file: %s\n", input_file);
    analyze_text(input_file, &stats);

    save_results(&stats);
    printf("Results saved to: %s\n", RESULT_FILE);

    return 0;
}
