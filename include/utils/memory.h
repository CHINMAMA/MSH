#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void *MSH_Malloc(size_t buf_size);
void *MSH_Calloc(size_t n, size_t size);
void *MSH_Realloc(void *buf, size_t buf_size);
int MSH_Strcat(char **dest, const char *src, size_t *dest_cap, size_t *dest_len, size_t n);

#endif