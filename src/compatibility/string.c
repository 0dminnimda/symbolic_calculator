#include <stdlib.h>
#include <string.h>


char *string_duplicate(const char *source) {
    size_t len = strlen(source) + 1;
    char *copy = (char *)malloc(len * sizeof(char));
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, source, len);
    return copy;
}

char *string_duplicate_length(const char *source, size_t length) {
    char *copy = (char *)malloc(length * sizeof(char));
    if (copy == NULL) {
        return NULL;
    }
    strncpy(copy, source, length);
    return copy;
}
