#include "memory.h"
#include <stdlib.h>
#include "io.h"
#include <string.h>
#include <stdint.h>

#define REALLOC_GROW(a) (1 + (a) * 3 / 2)

void *MSH_Malloc(size_t buf_size)
{
    if (buf_size == 0)
        return NULL;

    void *buf = malloc(buf_size);
    
    if (!buf)
    {
        MSH_perror("Malloc failed. Buffer size: %ld\n", buf_size);
        exit(EXIT_FAILURE);
    }
    
    return buf;
}

void *MSH_Calloc(size_t n, size_t size)
{
    if (n == 0)
        return NULL;

    void *buf = calloc(n, size);
    
    if (!buf)
    {
        MSH_perror("Malloc failed. Buffer size: %ld\n", n);
        exit(EXIT_FAILURE);
    }
    
    return buf;
}

void *MSH_Realloc(void *buf, size_t buf_size)
{
    void *new_buf = realloc(buf, buf_size);
    
    if (new_buf == NULL && buf_size != 0)
    {
        MSH_perror("Realloc failed.\n");
        exit(EXIT_FAILURE);
    }

    return new_buf;
}

/*
** Wrapper for strncat. Reallocates memory for dest if needed.
*/
int MSH_Strcat(char **dest, const char *src, size_t *dest_cap, size_t *dest_len, size_t n) {
    if (!dest || !src || !dest_cap || !dest_len) {
        return -1;
    }
    
    if (n == 0) { return 0; }

    char *_dest = *dest;
    size_t _dest_len = *dest_len;
    size_t _dest_cap = *dest_cap;
    size_t sl = strlen(src);
    size_t src_len = n < sl ? n : sl;

    if (_dest_len > SIZE_MAX - src_len - 2)
        return -1;

    if (!_dest) {
        size_t new_cap = 
            MSH_BUFSIZE > _dest_len + src_len ?
            MSH_BUFSIZE : REALLOC_GROW(_dest_len + src_len);
        _dest = MSH_Malloc(new_cap * sizeof(*_dest));
        *dest_cap = new_cap;
        _dest_cap = new_cap;
        *dest_len = 0;
        _dest_len = 0;
    }

    if (_dest_cap < _dest_len + src_len + 2) {
        size_t new_cap = REALLOC_GROW(_dest_len + src_len);
        _dest = MSH_Realloc(_dest, new_cap * sizeof(*_dest));
        *dest_cap = new_cap;
    }
    
    memcpy(_dest + _dest_len, src, src_len);
    _dest[_dest_len + src_len] = '\0';
    *dest = _dest;
    *dest_len = _dest_len + src_len;

    return 0;
}