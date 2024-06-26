#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compatibility/read_line.h"
#include "parser.h"
#include "sum_of_product.h"

bool parse_sum_of_product_with_errors(const char *str, SumOfProducts *result) {
    const char *error;
    if (!parse_sum_of_product_from_string(str, result, &error)) {
        fprintf(stderr, "started at: '%100s'\n", str);
        fprintf(stderr, "stopped at: '%100s'\n", error);
        // fprintf(stderr, "^\n");
        fprintf(stderr, "Invalid equation\n");
        return false;
    }
    return true;
}

int main(void) {
    char operation;
    char *operation_str = read_line();
    if (operation_str == NULL) {
        fprintf(stderr, "Could not read operation\n");
        return 1;
    }
    operation_str = (char *)skip_whitespaces(operation_str);
    operation = *operation_str;
    if (operation != '+' && operation != '-' && operation != '*' && operation != '=') {
        fprintf(stderr, "Invalid operation, expected one of [+-*=]\n");
        return 1;
    }
    free(operation_str);

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

#ifdef DEBUG_MAIN
    printf("Read (%s) %c (%s)\n", equation1, operation, equation2);
#endif

    SumOfProducts sum1;
    SumOfProducts_construct(&sum1);
    if (!parse_sum_of_product_with_errors(equation1, &sum1)) {
        return 1;
    }
    free(equation1);

    SumOfProducts sum2;
    SumOfProducts_construct(&sum2);
    if (!parse_sum_of_product_with_errors(equation2, &sum2)) {
        return 1;
    }
    free(equation2);

#ifdef DEBUG_MAIN
    printf("Parsed (");
    SumOfProducts_fprint(&sum1, stdout);
    printf(") %c (", operation);
    SumOfProducts_fprint(&sum2, stdout);
    printf(")\n");
#endif

    switch (operation) {
        case '+': {
            SumOfProducts_inplace_add_sub(&sum1, &sum2, false);
            SumOfProducts_fprint(&sum1, stdout);
            printf("\n");
            break;
        }
        case '-': {
            SumOfProducts_inplace_add_sub(&sum1, &sum2, true);
            SumOfProducts_fprint(&sum1, stdout);
            printf("\n");
            break;
        }
        case '*': {
            SumOfProducts result;
            SumOfProducts_multiply(&sum1, &sum2, &result);
            SumOfProducts_fprint(&result, stdout);
            printf("\n");
            SumOfProducts_destruct(&result);
            break;
        }
        case '=': {
            if (SumOfProducts_are_equal(&sum1, &sum2)) {
                printf("equal\n");
            } else {
                printf("not equal\n");
            }
            break;
        }
        default: printf("Operation not implemented '%c'\n", operation); break;
    }

    SumOfProducts_destruct(&sum2);
    SumOfProducts_destruct(&sum1);

    return 0;
}
