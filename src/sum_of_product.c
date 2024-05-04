#include "sum_of_product.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void Term_construct(Term *self, size_t variable_index, Term *next_term) {
    self->variable_index = variable_index;
    self->next = next_term;
}
void Term_destruct(Term *self) {}

void Product_construct(Product *self, long coefficient, Product *next_product) {
    self->coefficient = coefficient;
    self->terms = NULL;
    self->next = next_product;
}
void Product_destruct(Product *self) {
    for_list_my_next(Term *, term, self->terms) {
        Term_destruct(term);
        Term *next_term = term->next;
        free(term);
        term = next_term;
    }
}
void Product_fprint(
    Product *self, FILE *stream, Variables *variables, bool printed_after_other_products
) {
    bool first_term = true;

    if (printed_after_other_products) {
        fprintf(stream, " ");
    }

    if (self->coefficient == 1) {
        if (printed_after_other_products) {
            fprintf(stream, "+");
        }
    } else if (self->coefficient == -1) {
        fprintf(stream, "-");
    } else {
        if (printed_after_other_products) {
            fprintf(stream, "%+ld", self->coefficient);
        } else {
            fprintf(stream, "%ld", self->coefficient);
        }
        first_term = false;
    }

    for_list(Term *, term, self->terms) {
        if (!first_term) {
            fprintf(stream, "*");
        }
        String_fprint(&variables->data[term->variable_index], stream);
        first_term = false;
    }
}
void Product_insert_term(Product *self, Term *term) {
    term->next = self->terms;
    self->terms = term;
}
bool Product_are_terms_equal(
    const Product *self, const Product *other, size_t length_of_variables,
    long *array_length_of_variables
) {
    memset(array_length_of_variables, 0, length_of_variables * sizeof(long));
    for_list(Term *, term, self->terms) { array_length_of_variables[term->variable_index] += 1; }
    for_list(Term *, term, other->terms) { array_length_of_variables[term->variable_index] -= 1; }
    for (size_t i = 0; i < length_of_variables; i++) {
        if (array_length_of_variables[i] != 0) {
            return false;
        }
    }
    return true;
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
    for_list_my_next(Product *, product, self->products) {
        Product_destruct(product);
        Product *next_product = product->next;
        free(product);
        product = next_product;
    }
}
void SumOfProducts_fprint(SumOfProducts *self, FILE *stream) {
    bool first_product = true;
    for_list(Product *, product, self->products) {
        Product_fprint(product, stream, &self->variables, !first_product);

        first_product = false;
    }
}
void SumOfProducts_insert_product(SumOfProducts *self, Product *product) {
    product->next = self->products;
    self->products = product;
}
Product *SumOfProducts_remove_next_product(SumOfProducts *self, Product *prev_product) {
    /*
    If we want to delete a product inbewteen the others - all good and usual.
    But to make it simple to use, we want to handle the case when
    the user want to delete the head product as the middlle.
    In this case there's no previous product (it's NULL) and
    also we have to work with the SOP as the holder of the head pointer.
    */
    if (prev_product == NULL) {
        Product *product = self->products;
        if (product == NULL) return NULL;

        self->products = product->next;
        Product_destruct(product);
        free(product);
        return self->products;
    }

    if (prev_product->next == NULL) return NULL;

    Product *next = prev_product->next;
    prev_product->next = next->next;
    Product_destruct(next);
    free(next);
    return prev_product->next;
}
void SumOfProducts_remove_zero_coefficient_products(SumOfProducts *self) {
    Product *prev_product = NULL;
    for_list_my_next(Product *, product, self->products) {
        Product *next_product;
        if (product->coefficient == 0) {
            next_product = SumOfProducts_remove_next_product(self, prev_product);
        } else {
            next_product = product->next;
        }
        prev_product = product;
        product = next_product;
    }
}
void SumOfProducts_add_sub_destructive(SumOfProducts *self, SumOfProducts *other, bool is_sub) {
    size_t *index_map = malloc((self->variables.size + other->variables.size) * sizeof(size_t));
    for (size_t i = 0; i < other->variables.size; ++i) {
        index_map[i] = Variables_insert(&self->variables, &other->variables.data[i]);
    }

    // destructive: sets the other SOP's incides to be the same as self's
    for_list(Product *, product, other->products) {
        for_list(Term *, term, product->terms) {
            term->variable_index = index_map[term->variable_index];
        }
    }

    long *array_length_of_variables = malloc(self->variables.size * sizeof(long));

    for_list(Product *, product1, self->products) {
        for_list(Product *, product2, other->products) {
            if (Product_are_terms_equal(
                    product1, product2, self->variables.size, array_length_of_variables
                )) {
                if (is_sub) {
                    product1->coefficient -= product2->coefficient;
                } else {
                    product1->coefficient += product2->coefficient;
                }
                // canonical SOP don't contain repeating products with the same terms
                // so we won't find anything further to add
                break;
            }
        }
    }

    SumOfProducts_remove_zero_coefficient_products(self);

    free(array_length_of_variables);
    free(index_map);
}
