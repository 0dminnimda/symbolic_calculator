#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "sum_of_product.h"

typedef struct {
    const char *str;
    SumOfProducts *sop;
} Parser;

const char *skip_whitespaces(const char *str) {
    for (; *str != '\0'; ++str) {
        if (!isspace(*str)) {
            return str;
        }
    }
    return str;
}

bool parse_long(Parser *parser, long *result) {
    /*
        long: [0-9]+
    */

    char *endptr = NULL;
    long value = strtol(parser->str, &endptr, 10);

    if (endptr == parser->str) {
#ifdef DEBUG_PARSER
        printf("parse_long: didn't parse a thing\n");
#endif
        return false;
    }
    // if (endptr == NULL) {
    //     // fprintf(stderr, "Not an integer\n");
    //     return false;
    // }
    if (errno == ERANGE) {
#ifdef DEBUG_PARSER
        fprintf(stderr, "The number is out of range for a long\n");
#endif
        return false;
    }

    parser->str = endptr;
    *result = value;
    return true;
}

bool parse_term(Parser *parser, Term *result, bool discard) {
    /*
        term: [a-zA-Z_][a-zA-Z0-9_]*
    */

    const char *start = parser->str;

    if (!isalpha(*parser->str) && *parser->str != '_') {
#ifdef DEBUG_PARSER
        printf("parse_term, first char is wrong\n");
#endif
        return false;
    }
    ++parser->str;

    for (; *parser->str != '\0'; ++parser->str) {
        if (!isalnum(*parser->str) && *parser->str != '_') break;
    }

    if (discard) {
#ifdef DEBUG_PARSER
        printf("parse_term: discard %.*s\n", (int)(parser->str - start), start);
#endif
        return true;
    }

    String variable;
    // it's ok to remove constness from start, Variables_insert does not modify the string
    String_construct_slice(&variable, (char *)start, parser->str - start);
    // (void)String_hash(&variable); // AWFULL!!
#ifdef DEBUG_PARSER
    printf("parse_term, matched '");
    String_fprint(&variable, stdout);
    printf("' (%zu)\n", variable.length);
#endif
    result->variable_index = Variables_insert(&parser->sop->variables, &variable);
    // don't destruct variable, since it's a slice, it does not own memory

    return true;
}

bool parse_product(Parser *parser, Product *result) {
    /*
        product: (long '*')? term ('*' term)*
    */
    parser->str = skip_whitespaces(parser->str);
    if (*parser->str == '\0') {
#ifdef DEBUG_PARSER
        printf("parse_product, found end of string\n");
#endif
        return false;
    }

    bool first = true;
    long coefficient = 1;
    if (parse_long(parser, &coefficient)) {
        first = false;
    }
#ifdef DEBUG_PARSER
    else {
        printf("parse_product, no parse_long for '%s'\n", parser->str);
    }
#endif
    result->coefficient *= coefficient;

    while (1) {
        parser->str = skip_whitespaces(parser->str);
        if (first) {
            if (*parser->str == '\0') {
#ifdef DEBUG_PARSER
                printf("parse_product: hit end of string\n");
#endif
                break;
            }
        } else {
            if (*parser->str != '*') {
#ifdef DEBUG_PARSER
                printf("parse_product: hit non '*' '%s'\n", parser->str);
#endif
                break;
            }

            ++parser->str;
            parser->str = skip_whitespaces(parser->str);
        }

        Term *term = malloc(sizeof(Term));
        Term_construct(term, -1);
        // If we saw that product with coeff = 0,
        // we still need to parse all individual terms,
        // but not add them to the product
        if (!parse_term(parser, term, result->coefficient == 0)) {
#ifdef DEBUG_PARSER
            printf("parse_product, bad Term\n");
#endif
            Term_destruct(term);
            free(term);
            return false;
        }
        if (result->coefficient == 0) {
#ifdef DEBUG_PARSER
            printf("parse_product: result->coefficient == 0\n");
#endif
            Term_destruct(term);
            free(term);
        } else {
            Product_insert_term(result, &parser->sop->variables, term);
        }

        first = false;
    }
    return true;
}

bool parse_sum_of_product(Parser *parser, SumOfProducts *result) {
    /*
        sum_of_product: ('-'|'+')? product (('-'|'+') product)*
    */
    bool first = true;
    while (1) {
        parser->str = skip_whitespaces(parser->str);
        if (*parser->str == '\0') {
#ifdef DEBUG_PARSER
            printf("parse_product: hit end of string\n");
#endif
            break;
        }

        long coefficient = 1;
        if (*parser->str == '-') {
            coefficient = -1;
            ++parser->str;
        } else if (*parser->str == '+') {
            // coefficient = 1
            ++parser->str;
        } else if (!first) {
#ifdef DEBUG_PARSER
            printf("parse_sum_of_product, not [+-] '%s'\n", parser->str);
#endif
            return false;
        }

        Product *product = malloc(sizeof(Product));
        Product_construct(product, coefficient);
        if (!parse_product(parser, product)) {
#ifdef DEBUG_PARSER
            printf("parse_sum_of_product, bad Product\n");
#endif
            Product_destruct(product);
            free(product);
            return false;
        }
        // If we parsed a product with coeff = 0,
        // we just don't add it to the sum
        if (product->coefficient == 0) {
#ifdef DEBUG_PARSER
            printf("parse_sum_of_product: product->coefficient == 0\n");
#endif
            Product_destruct(product);
            free(product);
        } else {
            SumOfProducts_insert_product(result, product);
        }

        first = false;
    }

    return true;
}

bool parse_sum_of_product_from_string(
    const char *str, SumOfProducts *result, const char **error_position
) {
    Parser parser = {str, result};
    bool ret = parse_sum_of_product(&parser, result);
    if (!ret) {
        *error_position = parser.str;
    }
    return ret;
}
