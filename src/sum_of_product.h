#pragma once

#ifndef SUM_OF_PRODUCT_H
#define SUM_OF_PRODUCT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "sized_string.h"

typedef struct Term {
    size_t variable_index;
    struct Term *next;
} Term;

typedef struct Product {
    long coefficient;
    Term *terms;
    size_t terms_count;
    size_t terms_hash;
    struct Product *next;
} Product;

typedef struct {
    String *data;
    size_t capacity;
    size_t size;
} Variables;

typedef struct {
    Product *products;
    Variables variables;
} SumOfProducts;

/*
SumOfProducts layout example:
    - 1 * Sailor * ... * airspeed_velocity_of_an_unladen_swallow
    + 200 * x * ... * x
    ...

products:
-> product(-1) -> term(2) -> ... -> term(1)
   |
   v
   product(200) -> term(1) -> ... -> term(1)
   |
   v
   ...

variables(...):
    [0]: "Sailor"
    [1]: "x"
    [2]: "airspeed_velocity_of_an_unladen_swallow"
    ...
*/

#define for_list(type, item, list) for (type item = list; item != NULL; item = item->next)
#define for_list_my_next(type, item, list) for (type item = list; item != NULL;)

void Term_construct(Term *self, size_t variable_index);
void Term_destruct(Term *self);
void Term_copy(const Term *self, Term *copy);

void Product_construct(Product *self, long coefficient);
void Product_destruct(Product *self);
void Product_fprint(
    const Product *self, FILE *stream, const Variables *variables, bool printed_after_other_products
);
void Product_copy(const Product *self, const Variables *variables, Product *copy);
void Product_insert_term(Product *self, const Variables *variables, Term *term);
bool Product_are_mapped_terms_equal(
    const Product *self, const Product *other, size_t length_of_variables,
    long *array_length_of_variables, const size_t *index_map_for_other
);

void Variables_construct(Variables *self, size_t capcity);
void Variables_destruct(Variables *self);
size_t Variables_find(const Variables *self, const String *string);
size_t Variables_insert(Variables *self, const String *string);

void SumOfProducts_construct(SumOfProducts *self);
void SumOfProducts_destruct(SumOfProducts *self);
void SumOfProducts_fprint(const SumOfProducts *self, FILE *stream);
void SumOfProducts_insert_product(SumOfProducts *self, Product *product);
Product *SumOfProducts_remove_next_product(SumOfProducts *self, Product *prev_product);
void SumOfProducts_remove_zero_coefficient_products(SumOfProducts *self);
void SumOfProducts_inplace_add_sub(SumOfProducts *self, const SumOfProducts *other, bool is_sub);
bool SumOfProducts_are_equal(const SumOfProducts *self, const SumOfProducts *other);

#endif  // SUM_OF_PRODUCT_H
