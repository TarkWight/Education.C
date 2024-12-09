#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

#include "solution.h"

int main() {
    srand(time(NULL));

    int size = 10;
    int* array = (int*)allocate_memory(size * sizeof(int));

    fill_array(array, size, 21, 10);

    // пример того, как работать с функциями.
    solution(array, size);

    return 0;
}
