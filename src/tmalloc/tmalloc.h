#ifndef TMALLOC_H
#define TMALLOC_H

#include <unistd.h>

void *malloc(size_t size);
void *calloc(size_t num, size_t nsize);
void *realloc(void *block, size_t size);
void free(void *block);

void print_mem_list();

#endif
