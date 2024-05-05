#include "sized_string.h"

#include <stdlib.h>
#include <string.h>

#include "compatibility/string.h"

#define min(a, b) ((a) < (b) ? (a) : (b))

ssize_t hash_djb2(const char *str, size_t length) {
    ssize_t hash = 5381;
    for (size_t i = 0; i < length; ++i) {
        hash = ((hash << 5) + hash) + str[length]; /* hash * 33 + c */
    }
    return hash;
}

void String_construct(String *self, const char *data) {
    self->length = strlen(data);
    self->data = string_duplicate_known_length(data, self->length);
    self->hash_ = 0;
}
void String_construct_known_length(String *self, const char *data, size_t length) {
    self->length = length;
    self->data = string_duplicate_known_length(data, self->length);
    self->hash_ = 0;
}
void String_construct_slice(String *self, char *data, size_t length) {
    self->length = length;
    self->data = data;
    self->hash_ = 0;
}
void String_destruct(String *self) { free(self->data); }
void String_copy(const String *self, String *copy) {
    copy->length = self->length;
    copy->data = string_duplicate_known_length(self->data, self->length);
    copy->hash_ = self->hash_;
}
int String_fprint(const String *self, FILE *stream) {
    fprintf(stream, "%.*s", (int)self->length, self->data);
    return 0;
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
// AWFUL
// bool String_are_equal(const String *self, const String *other) {
//     if (self->hash_ != 0 && other->hash_ != 0 && self->hash_ != other->hash_) {
//         return false;
//     } else {
//         return String_compare(self, other) == 0;
//     }
// }
size_t String_hash(String *self) {
    if (self->hash_ == 0) {
        self->hash_ = hash_djb2(self->data, self->length);
        if (self->hash_ == 0) {
            self->hash_ = 1;
        }
    }
    return self->hash_;
}
