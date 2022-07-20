#include "Utility.hpp"

#include <cstdio>
#include <cstring>

// strndup with C++ memory allocation
char *cxx_strndup(const char *buf, size_t len)
{
    char *dest = 0;
    if (buf)
    {
        dest = new char[len + 1];
        memcpy(dest, buf, len);
        dest[len + 1] = '\0';
    }
    return dest;
}

void print_bytes(const char *buf, size_t len)
{
    for (size_t i = 0; i < len; i++)
        printf("'%c' '%d'\n", buf[i], buf[i]);
}

void *memdup(const void *src, size_t n)
{
    char *cdest = new char[n];
    const char *csrc = static_cast<const char*>(src);
    for (size_t i = 0; i < n; i++)
        cdest[i] = csrc[i];
    return static_cast<void*>(cdest);
}
