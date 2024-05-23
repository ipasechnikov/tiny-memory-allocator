#include <stdio.h>
#include <stdlib.h>

#include "tmalloc/tmalloc.h"

int main(int argc, char const *argv[]) {
  void *block = malloc(100);
  print_mem_list();
  free(block);
  print_mem_list();
  return 0;
}
