#include "solution.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int* reverse_negatives(const int* arr, int size) {
    int* result = (int*)allocate_memory(size * sizeof(int));

    int count = 0;
    for (int i = 0; i < size; i++) {
        result[i] = arr[i];
        if (arr[i] < 0) {
            count++;
        }
    }

    if (count > 0) {
        int* temp = (int*)allocate_memory(count * sizeof(int));

        for (int i = size - 1, j = 0; i >= 0; i--) {
            if (arr[i] < 0) {
                temp[j++] = arr[i];
            }
        }

        for (int i = 0, j = 0; i < size; i++) {
            if (result[i] < 0) {
                result[i] = temp[j++];
            }
        }
        free(temp);
    }
    return result;
}

int* reverse_odd(const int* arr, int size) {
    int* result = (int*)allocate_memory(size * sizeof(int));

    int count = 0;
    for (int i = 0; i < size; i++) {
        result[i] = arr[i];
        if (arr[i] % 2 != 0) {
            count++;
        }
    }

    if (count > 0) {
        int* temp = (int*)allocate_memory(count * sizeof(int));

        for (int i = size - 1, j = 0; i >= 0; i--) {
            if (arr[i] % 2 != 0) {
                temp[j++] = arr[i];
            }
        }

        for (int i = 0, j = 0; i < size; i++) {
            if (result[i] % 2 != 0) {
                result[i] = temp[j++];
            }
        }
        free(temp);
    }
    return result;
}

int* reverse_non_zero(const int* arr, int size) {
    int* result = (int*)allocate_memory(size * sizeof(int));

    int count = 0;
    for (int i = 0; i < size; i++) {
        result[i] = arr[i];
        if (arr[i] != 0) {
            count++;
        }
    }

    if (count > 0) {
        int* temp = (int*)allocate_memory(count * sizeof(int));

        for (int i = size - 1, j = 0; i >= 0; i--) {
            if (arr[i] != 0) {
                temp[j++] = arr[i];
            }
        }

        for (int i = 0, j = 0; i < size; i++) {
            if (result[i] != 0) {
                result[i] = temp[j++];
            }
        }
        free(temp);
    }
    return result;
}

int* reverse_positive(const int* arr, int size) {
    int* result = (int*)allocate_memory(size * sizeof(int));

    int count = 0;
    for (int i = 0; i < size; i++) {
        result[i] = arr[i];
        if (arr[i] > 0) {
            count++;
        }
    }

    if (count > 0) {
        int* temp = (int*)allocate_memory(count * sizeof(int));

        for (int i = size - 1, j = 0; i >= 0; i--) {
            if (arr[i] > 0) {
                temp[j++] = arr[i];
            }
        }

        for (int i = 0, j = 0; i < size; i++) {
            if (result[i] > 0) {
                result[i] = temp[j++];
            }
        }
        free(temp);
    }
    return result;
}

int* separate_odd(const int* arr, int size) {
    int* result = (int*)allocate_memory(size * sizeof(int));

    int odd_index = 0;
    int even_index = size - 1;

    for (int i = 0; i < size; i++) {
        if (arr[i] % 2 != 0) {
            result[odd_index++] = arr[i];
        } else {
            result[even_index--] = arr[i];
        }
    }
    return result;
}


void* allocate_memory(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        printf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void print_array(const int* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void fill_array(int* arr, int size, int left_range, int right_range) {
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % left_range - right_range;
    }
}

void solution(const int* array, int size) {
    printf("Source array:\n");
    print_array(array, size);

    int choice;
    int* modified_array = NULL;
    printf("Choose an option:\n");
    while (1) {
        printf("1: (2) Array with negative elements in reverse order:\n");
        printf("2: (7) Array with odd elements in reverse order:\n");
        printf("3: (14) Array with non-zero elements in reverse order:\n");
        printf("4: (15) Array with positive elements in reverse order:\n");
        printf("5: (19) Array with odd numbers in front:\n");
        printf("0: Exit\n");
        scanf("%d", &choice);


        switch (choice) {
        case 1:
            modified_array = reverse_negatives(array, size);
            print_array(modified_array, size);
            break;
        case 2:
            modified_array = reverse_odd(array, size);
            print_array(modified_array, size);
            break;
        case 3:
            modified_array = reverse_non_zero(array, size);
            print_array(modified_array, size);
            break;
        case 4:
            modified_array = reverse_positive(array, size);
            print_array(modified_array, size);
            break;
        case 5:
            modified_array = separate_odd(array, size);
            print_array(modified_array, size);
            break;

        default:
            printf("Wrong choice\nChoose one of the following options:\n");
        }
    }

    free(modified_array);
}
