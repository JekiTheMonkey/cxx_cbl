#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstddef>

/// @brief Duplicate given buffer into a new allocated memory.
/// @param buf Data to duplicate.
/// @param len Characters to duplicate.
/// @details The cxx_strndup() allocates len+1 bytes, copies given memory into
/// it, adds the terminating-null and returns the string. Memory is obtained
/// with new operator, and can be freed with delete operator.
char *cxx_strndup(const char *buf, size_t len);

/// @brief Duplicate given buffer into a new allocated memory.
/// @param src Memory to copy.
/// @param n Bytes to copy.
/// @details The memdup() allocates n bytes, copies given memory into it and
/// returns the allocated memory. Memory is obtained with new operator, and can
/// be freed with delete operator.
void *memdup(const void *src, size_t n);

void print_bytes(const char *buf, size_t len);

/// @brief Free null terminating array of pointers.
/// @param matrix Array of pointers to free.
/// @details The deleteMatrix<T>() frees each matrix element and the matrix
/// itself in case of valid pointer. Memory is freed with delete operator.
template <class T>
void deleteMatrix(T **matrix)
{
    if (matrix)
    {
        for (size_t i = 0; matrix[i]; i++)
            delete matrix[i];
        delete[] matrix;
    }
}

#endif // UTILITY_HPP
