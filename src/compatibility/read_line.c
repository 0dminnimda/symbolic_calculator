#include "read_line.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

char *fread_line(FILE *stream) {
    size_t len = 0;
    size_t cap = 8;  // does not count the '\0'
    char *buf = malloc((cap + 1) * sizeof(char));

    while (1) {
        for (; len < cap; ++len) {
            int c = buf[len] = fgetc(stream);
            if (c == EOF) goto finish_eof;
            if (c == '\n') goto finish;
        }

        cap *= 2;
        char *new_buf = realloc(buf, (cap + 1) * sizeof(char));
        if (new_buf == NULL) {
            free(buf);
            return NULL;
        }
        buf = new_buf;
    }

finish_eof:
    if (len == 0) {
        free(buf);
        return NULL;
    }
finish:
    buf[len] = '\0';
    return buf;
}

char *read_line(void) { return fread_line(stdin); }
