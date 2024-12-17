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
    for (int i = 0; i < stats->word_count; i++) {
        if (strcmp(stats->word_frequency[i].word, word) == 0) {
            stats->word_frequency[i].frequency++;
            return;
        }
    }

    strcpy(stats->word_frequency[stats->word_count].word, word);
    stats->word_frequency[stats->word_count].frequency = 1;
    stats->word_count++;
}

int compare_words(const void *a, const void *b) {
    return strcmp(((WordFrequency *)a)->word, ((WordFrequency *)b)->word);
}

void analyze_text(const char *filename, TextStats *stats) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char line[MAX_LINE_LEN];
    stats->total_paragraphs = 0;
    stats->total_sentences = 0;
    stats->total_words = 0;

    int prev_line_empty = 0;
    int current_paragraph_started = 0;

    while (fgets(line, sizeof(line), file)) {
        int line_len = strlen(line);

        if (line_len == 1 && line[0] == '\n') {
            if (current_paragraph_started) {
                stats->total_paragraphs++;
                current_paragraph_started = 0;
            }
            prev_line_empty = 1;
            continue;
        }

        prev_line_empty = 0;

        if (!current_paragraph_started) {
            stats->total_paragraphs++;
            current_paragraph_started = 1;
        }

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

    fprintf(file, "===========================\n");
    fprintf(file, "General Text Statistics\n");
    fprintf(file, "===========================\n");
    fprintf(file, "Paragraphs: %d\n", stats->total_paragraphs);
    fprintf(file, "Sentences: %d\n", stats->total_sentences);
    fprintf(file, "Words: %d\n", stats->total_words);
    fprintf(file, "Average words per sentence: %.2f\n\n",
            (double)stats->total_words / stats->total_sentences);

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
            printf("Word '%s' found. Frequency: %d\n", word, stats->word_frequency[mid].frequency);
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

    printf("Analyzing file: %s\n", input_file);
    analyze_text(input_file, &stats);

    save_results(&stats);
    printf("Results saved to: %s\n", RESULT_FILE);

    char search_word_str[100];
    printf("Enter word to search: ");
    scanf("%s", search_word_str);
    search_word(&stats, search_word_str);

    free(stats.word_frequency);

    return 0;
}
/*

Hello.

World!

===========================
General Text Statistics
===========================
Paragraphs: 1
Sentences: 1
Words: 2
Average words per sentence: 2.00

===========================
Character Frequency
===========================
 : 1
!: 1
,: 1
H: 1
W: 1
d: 1
e: 1
l: 3
o: 2
r: 1

===========================
Word Frequency
===========================
hello: 0.50
world: 0.50

*/
