#include "sum_of_product.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "compatibility/string.h"

void Term_construct(Term *self, size_t variable_index, Term *next_term) {
    self->variable_index = variable_index;
    self->next_term = next_term;
}
void Term_destruct(Term *self) {}

void Product_construct(Product *self, long coefficient, Product *next_product) {
    self->coefficient = coefficient;
    self->terms = NULL;
    self->next_product = next_product;
}
void Product_destruct(Product *self) {
    for (Term *term = self->terms; term != NULL;) {
        Term_destruct(term);
        Term *next_term = term->next_term;
        free(term);
        term = next_term;
    }
}
void Product_insert_term(Product *self, Term *term) {
    term->next_term = self->terms;
    self->terms = term;
}

void Variables_construct(Variables *self, size_t capcity) {
    self->capcity = capcity;
    self->size = 0;
    self->data = malloc(capcity * sizeof(char *));
}
void Variables_destruct(Variables *self) {
    for (size_t i = 0; i < self->size; ++i) {
        free(self->data[i]);
    }
    free(self->data);
}
size_t Variables_find(Variables *self, const char *variable, size_t length) {
    for (size_t i = 0; i < self->size; ++i) {
        if (strncmp(self->data[i], variable, length) == 0) {
            return i;
        }
    }
    return SIZE_MAX;
}
size_t Variables_insert(Variables *self, const char *variable, size_t length) {
    size_t index = Variables_find(self, variable, length);
    if (index != SIZE_MAX) {
        return index;
    }

    if (self->size >= self->capcity) {
        self->capcity = self->capcity * 2 + 1;
        self->data = realloc(self->data, self->capcity * sizeof(char *));
    }

    self->data[self->size] = string_duplicate_length(variable, length);
    return self->size++;
}

void SumOfProducts_construct(SumOfProducts *self) {
    Variables_construct(&self->variables, 0);
    self->products = NULL;
}
void SumOfProducts_destruct(SumOfProducts *self) {
    Variables_destruct(&self->variables);
    for (Product *product = self->products; product != NULL;) {
        Product_destruct(product);
        Product *next_product = product->next_product;
        free(product);
        product = next_product;
    }
}
void SumOfProducts_insert_product(SumOfProducts *self, Product *product) {
    product->next_product = self->products;
    self->products = product;
}
void SumOfProducts_fprint(SumOfProducts *self, FILE *stream) {
    bool first_product = true;
    for (Product *product = self->products; product != NULL; product = product->next_product) {
        bool first_term = true;

        if (!first_product) {
            fprintf(stream, " ");
        }

        if (product->coefficient == 1) {
            if (!first_product) {
                fprintf(stream, "+");
            }
        } else if (product->coefficient == -1) {
            fprintf(stream, "-");
        } else {
            fprintf(stream, "%+ld", product->coefficient);
            first_term = false;
        }

        for (Term *term = product->terms; term != NULL; term = term->next_term) {
            if (!first_term) {
                fprintf(stream, "*");
            }
            fprintf(stream, "%s", self->variables.data[term->variable_index]);
            first_term = false;
        }

        first_product = false;
    }
}
