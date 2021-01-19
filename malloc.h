#ifndef MALLOC_H
#define MALLOC_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define M_CHUNK getpagesize()
#define HEADER_S sizeof(size_t)
#define HEADER_C (sizeof(size_t) * 2)
#define ALIGN HEADER_S
#define PADDING(mem_asked) ((ALIGN - (mem_asked % ALIGN)) % ALIGN)

void *naive_malloc(size_t size);
void *_malloc(size_t size);
void _free(void *ptr);
void *_calloc(size_t nmemb, size_t size);
void *_realloc(void *ptr, size_t size);
#endif
