#pragma once

#ifndef SUM_OF_PRODUCT_H
#define SUM_OF_PRODUCT_H

#include <stddef.h>
#include <stdio.h>

typedef struct Term {
    size_t variable_index;
    struct Term *next_term;
} Term;

typedef struct Product {
    long coefficient;
    Term *terms;
    struct Product *next_product;
} Product;

typedef struct {
    char **data;
    size_t capcity;
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

void Term_construct(Term *self, size_t variable_index, Term *next_term);
void Term_destruct(Term *self);

void Product_construct(Product *self, long coefficient, Product *next_product);
void Product_destruct(Product *self);
void Product_insert_term(Product *self, Term *term);

void Variables_construct(Variables *self, size_t capcity);
void Variables_destruct(Variables *self);
size_t Variables_find(Variables *self, const char *variable, size_t length);
size_t Variables_insert(Variables *self, const char *variable, size_t length);

void SumOfProducts_construct(SumOfProducts *self);
void SumOfProducts_destruct(SumOfProducts *self);
void SumOfProducts_insert_product(SumOfProducts *self, Product *product);
void SumOfProducts_fprint(SumOfProducts *self, FILE *stream);

#endif  // SUM_OF_PRODUCT_H
