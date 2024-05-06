#pragma once

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "sum_of_product.h"

const char *skip_whitespaces(const char *str);
bool parse_sum_of_product_from_string(
    const char *str, SumOfProducts *result, const char **error_position
);

#endif  // PARSER_H
