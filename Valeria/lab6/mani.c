#include <stddef.h> // Для size_t

size_t my_strlen(const char *str) {
    size_t length = 0;

    // Проходим по строке, пока не встретим нулевой символ '\0'
    while (str[length] != '\0') {
        length++;
    }

    return length;
}

#include <stddef.h> // Для size_t
#include <stdlib.h> // Для malloc

void *my_memcpy2(const void *src, size_t size) {
    if (src == NULL || size == 0) {
        return NULL; // Если исходный указатель NULL или размер 0, возвращаем NULL
    }

    // Выделяем память
    void *dst = malloc(size);

    if (dst == NULL) {
        return NULL; // Если память не была выделена, возвращаем NULL
    }

    // Копируем данные из src в dst
    for (size_t i = 0; i < size; i++) {
        ((unsigned char *)dst)[i] = ((unsigned char *)src)[i];
    }

    return dst;
}

#include <stdio.h>

int main() {
    // Пример использования my_strlen
    const char *str = "Hello, world!";
    size_t len = my_strlen(str);
    printf("Length of string: %zu\n", len);

    // Пример использования my_memcpy2
    const char *src = "Data to copy";
    size_t size = my_strlen(src) + 1;  // +1 для учёта нулевого символа
    void *copied_data = my_memcpy2(src, size);

    if (copied_data != NULL) {
        printf("Copied data: %s\n", (char *)copied_data);
        free(copied_data); // Не забываем освободить память
    } else {
        printf("Memory allocation failed.\n");
    }

    return 0;
}
