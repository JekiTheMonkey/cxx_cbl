#include "Utility.hpp"

#include <cstdio>
#include <cstring>
#include <stdint.h>

char *cxx_strndup(const char *src, size_t n)
{
    char *dest = 0;
    if (src)
    {
        dest = new char[n + 1];
        memcpy(dest, src, n);
        dest[n + 1] = '\0';
    }
    return dest;
}

void hexdump(const void *data, size_t n)
{
    const uint16_t *cbuf = static_cast<const uint16_t*>(data);
    for (size_t i = 0; i < n; i++)
    {
        printf("%04x ", cbuf[i]);
        if (i > 0 && (i + 1) % 8 == 0)
            putchar(10);
    }
}

void *memdup(const void *src, size_t n)
{
    char *cdest = new char[n];
    const char *csrc = static_cast<const char*>(src);
    for (size_t i = 0; i < n; i++)
        cdest[i] = csrc[i];
    return static_cast<void*>(cdest);
}

void delete_str_array(char **array)
{
    if (array)
    {
        for (size_t i = 0; array[i]; i++)
            delete[] array[i];
        delete[] array;
    }
}
