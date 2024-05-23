#include "tmalloc.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>

// Used to align memory headers to 16 bytes
typedef char ALIGN[16];

union header {
  struct {
    size_t size;
    unsigned is_free;
    union header *next;
  } s;
  ALIGN stub;
};

typedef union header header_t;

// Allocated memory is stored as a linked list
header_t *head = NULL;
header_t *tail = NULL;

// Prevent multiple threads from trying to manage memory concurrently
pthread_mutex_t global_malloc_lock;

header_t *get_free_block(size_t size) {
  header_t *curr = head;
  while (curr) {
    if (curr->s.is_free && curr->s.size >= size) {
      return curr;
    } else {
      curr = curr->s.next;
    }
  }
  return NULL;
}

void *malloc(size_t size) {
  if (!size) {
    return NULL;
  }

  pthread_mutex_lock(&global_malloc_lock);

  // Try to find a first free block with a right size
  header_t *header = get_free_block(size);
  if (header) {
    header->s.is_free = 0;
    pthread_mutex_unlock(&global_malloc_lock);
    return (void *)(header + 1);
  }

  size_t total_size = sizeof(header_t) + size;
  void *block = sbrk(total_size);
  if (block == (void *)-1) {
    pthread_mutex_unlock(&global_malloc_lock);
    return NULL;
  }

  // Init a newly allocated header
  header = block;
  header->s.size = size;
  header->s.is_free = 0;
  header->s.next = NULL;

  // If it is the first allocated block, update memory head to point to a new
  // header
  if (!head) {
    head = header;
  }

  // If there is a tail, update it to point to a new header
  if (tail) {
    tail->s.next = header;
  }

  tail = header;

  pthread_mutex_unlock(&global_malloc_lock);
  return (void *)(header + 1);
}

void free(void *block) {
  if (!block) {
    return;
  }

  pthread_mutex_lock(&global_malloc_lock);

  header_t *header = (header_t *)block - 1;
  void *program_break = sbrk(0);

  // If block is at the end of the heap, then we can release it to OS
  if ((char *)block + header->s.size == program_break) {
    if (head == tail) { // Quick release if it is a single allocated block
      head = NULL;
      tail = NULL;
    } else { // Otherwise traverse the linked list in search of the last block
      header_t *tmp = head;
      while (tmp) {
        if (tmp->s.next == tail) {
          tmp->s.next = NULL;
          tail = tmp;
        }
        tmp = tmp->s.next;
      }
    }

    // System call to release memory to the OS
    sbrk(0 - sizeof(header_t) - header->s.size);

    pthread_mutex_unlock(&global_malloc_lock);
    return;
  }

  // Otherwise mark it as free and finish the memory free process
  header->s.is_free = 1;

  pthread_mutex_unlock(&global_malloc_lock);
}

void *calloc(size_t num, size_t nsize) {
  if (!num || !nsize) {
    return NULL;
  }

  size_t size = num * nsize;

  // Check multiplication overflow
  if (nsize != size / num) {
    return NULL;
  }

  void *block = malloc(size);
  if (!block) {
    return NULL;
  }

  memset(block, 0, size);
  return block;
}

void *realloc(void *block, size_t size) {
  if (!block || !size) {
    return malloc(size);
  }

  header_t *header = (header_t *)block - 1;
  if (header->s.size >= size) {
    return block;
  }

  void *ret = malloc(size);
  if (ret) {
    memcpy(ret, block, header->s.size);
    free(block);
  }

  return ret;
}

void print_mem_list() {
  header_t *curr = head;
  printf("head = %p, tail = %p \n", (void *)head, (void *)tail);
  while (curr) {
    printf("addr = %p, size = %lu, is_free = %u, next = %p\n", (void *)curr,
           curr->s.size, curr->s.is_free, curr->s.next);
    curr = curr->s.next;
  }
}
