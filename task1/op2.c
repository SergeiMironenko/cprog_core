// Динамическая подгрузка функций из библиотеки

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(int argc, char *argv[]) {
    float op1;
    void *ext_lib;
    void (*func)(float a);

    if (argc < 3) {
        printf("Input format: ./op <func> <arg1>\n");
        exit(EXIT_FAILURE);
    }

    ext_lib = dlopen("./libopdyn.so", RTLD_LAZY);
    if (ext_lib == NULL) {
        printf("%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    op1 = atof(argv[2]);
    func = dlsym(ext_lib, argv[1]);
    if (func == NULL) {
        printf("%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    (*func)(op1);

    exit(EXIT_SUCCESS);
}
