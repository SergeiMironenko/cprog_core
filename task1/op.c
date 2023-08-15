// Используется статическая или динамическая библиотека, подгруженная во время компиляции
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

extern void _sqr(float a);
extern void _sqrt(float a);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Input format: ./op <arg1>\n");
        exit(EXIT_FAILURE);
    }

    float op1 = atof(argv[1]);

    _sqr(op1);
    _sqrt(op1);

    exit(EXIT_SUCCESS);
}
