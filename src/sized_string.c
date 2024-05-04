#include "sized_string.h"

#include <stdlib.h>
#include <string.h>

#include "compatibility/string.h"


#define min(a, b) ((a) < (b) ? (a) : (b))

void String_construct(String *self, const char *data) {
    self->length = strlen(data);
    self->data = string_duplicate_known_length(data, self->length);
}
void String_construct_known_length(String *self, const char *data, size_t length) {
    self->length = length;
    self->data = string_duplicate_known_length(data, self->length);
}
void String_construct_slice(String *self, char *data, size_t length) {
    self->length = length;
    self->data = data;
}
void String_destruct(String *self) { free(self->data); }
void String_copy(const String *self, String *copy) {
    String_construct_known_length(copy, self->data, self->length);
}
int String_compare(const String *self, const String *other) {
    if (self->length < other->length) {
        return -1;
    } else if (self->length > other->length) {
        return 1;
    } else {
        return memcmp(self->data, other->data, self->length);
    }
}
int String_fprint(const String *self, FILE *stream) {
    fprintf(stream, "%*s", (int)self->length, self->data);
    return 0;
}
