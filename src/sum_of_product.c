#include "sum_of_product.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void Term_construct(Term *self, size_t variable_index) {
    self->variable_index = variable_index;
    self->next = NULL;
}
void Term_destruct(Term *self) { (void)self; }
void Term_copy(const Term *self, Term *copy) {
    copy->variable_index = self->variable_index;
    copy->next = NULL;
}

void Product_construct(Product *self, long coefficient) {
    self->coefficient = coefficient;
    self->terms = NULL;
    self->terms_count = 0;
    self->next = NULL;
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
    const Product *self, FILE *stream, const Variables *variables, bool printed_after_other_products
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
const Term *Product_get_last_term_const(const Product *self) {
    Term *last_term = NULL;
    for_list(Term *, term, self->terms) { last_term = term; }
    return last_term;
}
Term *Product_get_last_term(Product *self) {
    return (Term *)Product_get_last_term_const((const Product *)self);
}
void Product_extend_from(Product *self, const Product *source) {
    for_list(const Term *, term, source->terms) {
        Term *new_term = malloc(sizeof(Term));
        Term_copy(term, new_term);
        Product_insert_term(self, new_term);
    }
}
void Product_copy(const Product *self, Product *copy) {
    Product_construct(copy, self->coefficient);
    Product_extend_from(copy, self);
}
void Product_insert_term(Product *self, Term *term) {
    term->next = self->terms;
    self->terms = term;
    ++self->terms_count;
}
void Product_multiply_mapped(
    const Product *self, const Product *other, Product *result, const size_t *index_map_for_other
) {
    Product_copy(other, result);
    result->coefficient *= self->coefficient;
    for_list(Term *, term, result->terms) {
        term->variable_index = index_map_for_other[term->variable_index];
    }
    Product_extend_from(result, self);
}
bool Product_are_mapped_terms_equal(
    const Product *self, const Product *other, size_t variable_count, long *variable_powers,
    const size_t *index_map_for_other
) {
    if (self->terms_count != other->terms_count) return false;

    // We don't case about the order of variables,
    // we only care that there is the same amount of them in each product
    // so we just count the amount, veriables in self count up and in other - down
    // @XXX can we get read of this memset? will initializing the array using self->terms be
    // better/feasable?
    memset(variable_powers, 0, variable_count * sizeof(long));

    long unique_variable_count = 0;
    for_list(Term *, term, self->terms) {
        long *power = &variable_powers[term->variable_index];
        if (*power == 0) ++unique_variable_count;
        *power += 1;
    }

    for_list(Term *, term, other->terms) {
        long *power = &variable_powers[index_map_for_other[term->variable_index]];
        // If we are trying to decrease something that is already 0
        // => there's more of this variable in 'other'
        if (*power == 0) return false;
        *power -= 1;
        // If we hit 0 here, it means that the powers canceled out,
        // and the case where the power will get negative is handled above
        if (*power == 0) --unique_variable_count;
    }

    // If we canceled the same amount of unique variables we starting with
    // it means that the product terms are the same
    return unique_variable_count == 0;
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
size_t Variables_find(const Variables *self, const String *string) {
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
void Variables_copy(const Variables *self, Variables *copy) {
    copy->size = self->size;
    copy->capacity = self->size;
    copy->data = malloc(copy->capacity * sizeof(String));
    for (size_t i = 0; i < self->size; ++i) {
        String_copy(&self->data[i], &copy->data[i]);
    }
}

void SumOfProducts_construct(SumOfProducts *self) {
    Variables_construct(&self->variables, 0);
    self->products = NULL;
    self->products_count = 0;
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
void SumOfProducts_fprint(const SumOfProducts *self, FILE *stream) {
    if (self->products == NULL) {
        fprintf(stream, "0");
        return;
    }
    bool first_product = true;
    for_list(Product *, product, self->products) {
        Product_fprint(product, stream, &self->variables, !first_product);

        first_product = false;
    }
}
void SumOfProducts_insert_product(SumOfProducts *self, Product *product) {
    product->next = self->products;
    self->products = product;
    ++self->products_count;
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
        Product *current = self->products;
        if (current == NULL) return NULL;

        --self->products_count;
        self->products = current->next;
        Product_destruct(current);
        free(current);
        return self->products;
    } else {
        Product *current = prev_product->next;
        if (current == NULL) return NULL;

        --self->products_count;
        prev_product->next = current->next;
        Product_destruct(current);
        free(current);
        return prev_product->next;
    }
}
void SumOfProducts_remove_zero_coefficient_products(SumOfProducts *self) {
    Product *prev_product = NULL;
    for_list_my_next(Product *, product, self->products) {
        Product *next_product;
        if (product->coefficient == 0) {
            next_product = SumOfProducts_remove_next_product(self, prev_product);
        } else {
            next_product = product->next;
            prev_product = product;
        }
        product = next_product;
    }
}
void SumOfProducts_inplace_add_sub_Product_mapped_preallocated(
    SumOfProducts *self, const Product *product_other, const Variables *other_variables, bool is_sub,
    long *variable_powers, size_t *index_map, Product *self_products
) {
    bool performed_operation = false;
#ifdef DEBUG_SOP
    printf("From the first loop: ");
    Product_fprint(product_other, stdout, other_variables, false);
    printf("\n");
#endif

    for_list(Product *, product_self, self_products) {
#ifdef DEBUG_SOP
        printf("  From the second loop: ");
        Product_fprint(product_self, stdout, &self->variables, false);
        printf("\n");
#endif

        if (Product_are_mapped_terms_equal(
                product_self, product_other, self->variables.size, variable_powers, index_map
            )) {
#ifdef DEBUG_SOP
            printf("Matched two products:\n");
            Product_fprint(product_self, stdout, &self->variables, false);
            printf("\n");
            Product_fprint(product_other, stdout, other_variables, false);
            printf("\n");
#endif
            if (is_sub) {
                product_self->coefficient -= product_other->coefficient;
            } else {
                product_self->coefficient += product_other->coefficient;
            }
            performed_operation = true;
            // canonical SOP don't contain repeating products with the same terms
            // so we won't find anything further to add
            break;
        }
    }

    // if we did not find anything to act upon -
    // it means that the current term combination is not present in the self
    // add it
    if (!performed_operation) {
#ifdef DEBUG_SOP
        printf("Adding new product:\n");
        Product_fprint(product_other, stdout, other_variables, false);
        printf("\n");
#endif
        Product *new_product = malloc(sizeof(Product));
        Product_copy(product_other, new_product);
        if (is_sub) {
            new_product->coefficient = -new_product->coefficient;
        }
        // since we don't act destructive, we did not map the indices previously
        // we need to do it now instead, after the copy
        for_list(Term *, term, new_product->terms) {
            term->variable_index = index_map[term->variable_index];
        }
        SumOfProducts_insert_product(self, new_product);
    }
}
void SumOfProducts_inplace_add_sub(SumOfProducts *self, const SumOfProducts *other, bool is_sub) {
    size_t *index_map = malloc((self->variables.size + other->variables.size) * sizeof(size_t));
    for (size_t i = 0; i < other->variables.size; ++i) {
        index_map[i] = Variables_insert(&self->variables, &other->variables.data[i]);
    }

    long *variable_powers = malloc(self->variables.size * sizeof(long));

    Product *original_self_products = self->products;
    for_list(Product *, product_other, other->products) {
        // because all new products in the 'other' are unique
        // we don't need to check the newly added products
        // when adding any other product from 'other'
        SumOfProducts_inplace_add_sub_Product_mapped_preallocated(
            self, product_other, &other->variables, is_sub, variable_powers, index_map,
            original_self_products
        );
    }

    // @FIXME after removing it's possible that we will have some unused strings
    // but for now i think it's fine to leave it that way
    SumOfProducts_remove_zero_coefficient_products(self);

    free(variable_powers);
    free(index_map);
}
bool SumOfProducts_are_equal(const SumOfProducts *self, const SumOfProducts *other) {
    // shortcut: if the variables don't map into eachother, the SOPs are not the same
    // relies on the fact that all variables are used
    if (self->variables.size != other->variables.size) {
        return false;
    }
    size_t *index_map = malloc(self->variables.size * sizeof(size_t));
    for (size_t i = 0; i < other->variables.size; ++i) {
        size_t index = Variables_find(&self->variables, &other->variables.data[i]);
        if (index == SIZE_MAX) {
            free(index_map);
            return false;
        }
        index_map[i] = index;
    }

    long *variable_powers = malloc(self->variables.size * sizeof(long));

    bool result = true;
    for_list(Product *, product_self, self->products) {
        bool found_equal = false;
        for_list(Product *, product_other, other->products) {
            if (product_self->coefficient == product_other->coefficient &&
                Product_are_mapped_terms_equal(
                    product_self, product_other, self->variables.size, variable_powers, index_map
                )) {
                found_equal = true;
                // canonical SOP don't contain repeating products with the same terms
                // so we won't find anything further to process
                break;
            }
        }

        if (!found_equal) {
            result = false;
            break;
        }
    }

    free(variable_powers);
    free(index_map);
    return result;
}
void SumOfProducts_multiply(
    const SumOfProducts *self, const SumOfProducts *other, SumOfProducts *result
) {
    // @Optimization: 'self' and 'other' have shared products (S) and unique ones (U1, U2)
    // Using this we can decompose multiplication into predictable mini-muplitiplications:
    // (S + U1) * (S + U2) = S*S + S*(U1 + U2) + U1*U2
    // 1. S*S looks like (a + b + ... z) * (a + b + ... z) =
    //    = a*a + b*b + ... + z*z + 2*a*b + ... + 2*a*z + 2*b*z
    //    and all of them are unique because S are unique (as well as 'self' or 'other')
    // 2. (U1 + U2) are all also unique, since U1 and U2 are unique in set of all variables
    //    S and (U1 + U2) also don't share common products, so each term is unique,
    //    when it's added, no need to check if it exists
    // 3. U1*U2  are all also unique, so we hae the same situation where each product
    //    can be just added without checking
    // Since all 3 operands are unique (since they are cartesian products of pairwise unique sets)
    // So when adding those SOPs we can all the same just concatenate without checking

    // Example:
    // (S + U1) * (S + U2) = ((a + b) + (c)) * ((a + b) + (d)) =
    // = a*a + 2*a*b + b*b  +  a*d + b*d + c*a + c*b  +  c*d =
    // = (a * b) * (a * b)  +  d*(a + b) + c*(a + b)  +  c*d =
    // = (a + b) * (a + b)  +  (a + b) * (c + d) + (c) * (d) =
    // = S*S + S*(U1 + U2) + U1*U2

    // For now though a simple algorithm is used
    // - generate each possiple pair and add it to the total

    SumOfProducts_construct(result);
    Variables_copy(&self->variables, &result->variables);

    size_t *index_map = malloc((result->variables.size + other->variables.size) * sizeof(size_t));
    for (size_t i = 0; i < other->variables.size; ++i) {
        index_map[i] = Variables_insert(&result->variables, &other->variables.data[i]);
    }
    // YUCK!!
    size_t *identity_index_map = malloc(result->variables.size * sizeof(size_t));
    for (size_t i = 0; i < result->variables.size; ++i) {
        identity_index_map[i] = i;
    }

    long *variable_powers = malloc(result->variables.size * sizeof(long));

    for_list(Product *, product_self, self->products) {
#ifdef DEBUG_SOP
        printf("From the first loop (mult): ");
        Product_fprint(product_self, stdout, &self->variables, false);
        printf("\n");
#endif
        for_list(Product *, product_other, other->products) {
#ifdef DEBUG_SOP
            printf("  From the second loop (mult): ");
            Product_fprint(product_other, stdout, &other->variables, false);
            printf("\n");
#endif
            Product new_product;
            Product_multiply_mapped(product_self, product_other, &new_product, index_map);
#ifdef DEBUG_SOP
            printf("  New product in loop (mult): ");
            Product_fprint(&new_product, stdout, &result->variables, false);
            printf("\n");
#endif
            SumOfProducts_inplace_add_sub_Product_mapped_preallocated(
                result, &new_product, &result->variables, false, variable_powers, identity_index_map, result->products
            );
            Product_destruct(&new_product);
#ifdef DEBUG_SOP
            printf("  Total (mult): ");
            SumOfProducts_fprint(result, stdout);
            printf("\n");
#endif
        }
    }

    // @FIXME after removing it's possible that we will have some unused strings
    // but for now i think it's fine to leave it that way
    SumOfProducts_remove_zero_coefficient_products(result);

    free(variable_powers);
    free(identity_index_map);
    free(index_map);
}
void SumOfProducts_multiply_2(
    const SumOfProducts *self, const SumOfProducts *other, SumOfProducts *result
) {
    // 'self' and 'other' have shared products (S1, S2 - differing only with coefficients)
    // and unique ones (U1, U2)
    // Using this we can decompose multiplication into predictable mini-muplitiplications:
    // (S1 + U1) * (S2 + U2) = S1*S2 + S1*U2 + S2*U1 + U1*U2
    //
    // Since we have canonical form, S1 + U1 don't shared products as well as S2 + U2
    // Now, since S1 and S2 do share products with different coefficients, then
    // U1 don't share products with any of (S1, S2, U2), same for U2 and (S1, S2, U1)
    // This means that their multiplications ()
    //
    // 1. S*S looks like (c1*a + c2*b + ... cn*z) * (k1*a + k2*b + ... km*z) =
    //    = c1*k1*a*a + c2*k2*b*b + ... + cn*km*z*z +
    //    + (c1*k2 + c2*k1)*a*b + ... + (c1*km + cn*k1)*a*z + (c2*km + cn*k2)*b*z
    //    and all of them are unique because S1 and S2 are unique (as well as 'self' or 'other')
    // 2. S1*U2 are all also unique, since S1 and U2 also don't share common products,
    //    so each term is unique, when it's added, no need to check if it exists
    // 3. S2*U1 are all also unique, since U1 and U2 are unique in set of all variables
    //    S and (U1 + U2) also don't share common products, so each term is unique,
    //    when it's added, no need to check if it exists
    // 4. U1*U2  are all also unique, so we hae the same situation where each product
    //    can be just added without checking
    // Since all 3 operands are unique (since they are cartesian products of pairwise unique sets)
    // So when adding those SOPs we can all the same just concatenate without checking

    // Example:
    // (S + U1) * (S + U2) = ((a + b) + (c)) * ((a + b) + (d)) =
    // = a*a + 2*a*b + b*b  +  a*d + b*d + c*a + c*b  +  c*d =
    // = (a * b) * (a * b)  +  d*(a + b) + c*(a + b)  +  c*d =
    // = (a + b) * (a + b)  +  (a + b) * (c + d) + (c) * (d) =
    // = S*S + S*(U1 + U2) + U1*U2

    // (a)

    SumOfProducts_construct(result);
    Variables_copy(&self->variables, &result->variables);

    size_t *index_map = malloc((result->variables.size + other->variables.size) * sizeof(size_t));
    for (size_t i = 0; i < other->variables.size; ++i) {
        index_map[i] = Variables_insert(&result->variables, &other->variables.data[i]);
    }

    // The ideas is to have an array of flags, that say if a product is shared or not
    // We need to know/set only shared or unset only unique to have it fully complete
    long *variable_powers = malloc(result->variables.size * sizeof(long));
    bool *is_shared_product_self = malloc(self->products_count * sizeof(bool));
    memset(is_shared_product_self, 0xFF, self->products_count * sizeof(bool));
    bool *is_shared_product_other = malloc(other->products_count * sizeof(bool));
    memset(is_shared_product_other, 0, other->products_count * sizeof(bool));

    size_t product_index_self = 0;
    for_list(Product *, product_self, self->products) {
        bool matched = false;
        size_t product_index_other = 0;
        for_list(Product *, product_other, other->products) {
            if (Product_are_mapped_terms_equal(
                product_self, product_other, self->variables.size, variable_powers, index_map
            )) {
                // If we have found a matching product, it must be shared
                is_shared_product_other[product_index_other] = true;
                matched = true;
            }
            ++product_index_other;
        }
        if (!matched) {
            // If we have not found a matching product, it must be unique
            is_shared_product_self[product_index_self] = false;
        }
        ++product_index_self;
    }

    // @FIXME after removing it's possible that we will have some unused strings
    // but for now i think it's fine to leave it that way
    SumOfProducts_remove_zero_coefficient_products(result);

    free(variable_powers);
    free(identity_index_map);
    free(index_map);
}
