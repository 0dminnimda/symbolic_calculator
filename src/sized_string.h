#pragma once

#ifndef SIZED_STRING_H
#define SIZED_STRING_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
    char *data;
    size_t length;
} String;

void String_construct(String *self, const char *data);
void String_construct_known_length(String *self, const char *data, size_t length);
void String_construct_slice(String *self, char *data, size_t length);
void String_destruct(String *self);
int String_fprint(const String *self, FILE *stream);
void String_copy(const String *self, String *copy);
int String_compare(const String *self, const String *other);

#endif  // SIZED_STRING_H
