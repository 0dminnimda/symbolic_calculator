#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compatibility/read_line.h"


int main(void) {
    char operation;
    if (scanf("%c\n", &operation) <= 0) {
        fprintf(stderr, "Could not read operation\n");
        return 1;
    }
    if (operation != '+' && operation != '-' && operation != '*' && operation != '=') {
        fprintf(stderr, "Invalid operation, expected one of [+-*=]\n");
        return 1;
    }

    char *equation1 = read_line();
    if (equation1 == NULL) {
        fprintf(stderr, "\nCould not read first equation\n");
        return 1;
    }
    char *equation2 = read_line();
    if (equation2 == NULL) {
        fprintf(stderr, "\nCould not read second equation\n");
        return 1;
    }

    printf("Read (%s) %c (%s)\n", equation1, operation, equation2);
    return 0;
}
