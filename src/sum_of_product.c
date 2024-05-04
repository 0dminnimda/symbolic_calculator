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
    self->capacity = capcity;
    self->size = 0;
    self->data = malloc(capcity * sizeof(String));
}
void Variables_destruct(Variables *self) {
    for (size_t i = 0; i < self->size; ++i) {
        String_destruct(&self->data[i]);
    }
    free(self->data);
}
size_t Variables_find(Variables *self, const String *string) {
    for (size_t i = 0; i < self->size; ++i) {
        if (String_compare(&self->data[i], string) == 0) {
            return i;
        }
    }
    return SIZE_MAX;
}
size_t Variables_insert(Variables *self, const String *string) {
    size_t index = Variables_find(self, string);
    if (index != SIZE_MAX) {
        return index;
    }

    if (self->size >= self->capacity) {
        self->capacity = self->capacity * 2 + 1;
        self->data = realloc(self->data, self->capacity * sizeof(String));
    }

    String_copy(string, &self->data[self->size]);
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
            String_fprint(&self->variables.data[term->variable_index], stream);
            first_term = false;
        }

        first_product = false;
    }
}
