#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compatibility/read_line.h"
#include "sum_of_product.h"
#include "parser.c"


bool parse_sum_of_product_with_errors(const char *str, SumOfProducts *result) {
    const char *error;
    if (!parse_sum_of_product_from_string(str, result, &error)) {
        fprintf(stderr, "started at: '%s'\n", str);
        fprintf(stderr, "stopped at: '%s'\n", error);
        // fprintf(stderr, "^\n");
        fprintf(stderr, "Invalid equation\n");
        return false;
    }
    return true;
}


int main(void) {
    char operation;
    scanf("%*[ \t\r\f\v]");  // UB? - passing NULL
    if (scanf("%c", &operation) <= 0) {
        fprintf(stderr, "Could not read operation\n");
        return 1;
    }
    if (operation != '+' && operation != '-' && operation != '*' && operation != '=') {
        fprintf(stderr, "Invalid operation, expected one of [+-*=]\n");
        return 1;
    }
    scanf("%*[ \t\r\f\v]\n");  // UB? - passing NULL

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

    SumOfProducts sum1;
    SumOfProducts_construct(&sum1);

    SumOfProducts sum2;
    SumOfProducts_construct(&sum2);

    if (!parse_sum_of_product_with_errors(equation1, &sum1)) {
        return 1;
    }
    if (!parse_sum_of_product_with_errors(equation2, &sum2)) {
        return 1;
    }

    SumOfProducts_destruct(&sum2);
    SumOfProducts_destruct(&sum1);

    return 0;
}
