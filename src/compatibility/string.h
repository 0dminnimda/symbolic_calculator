#pragma once

#ifndef COMPAT_STRING_H
#define COMPAT_STRING_H

#include <stddef.h>

char *string_duplicate(const char *source);
char *string_duplicate_length(const char *source, size_t length);
char *string_duplicate_known_length(const char *source, size_t length);

#endif  // COMPAT_STRING_H
