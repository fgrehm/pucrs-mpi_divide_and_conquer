#include <stdio.h>
#include <stdlib.h>
#include "bubble_sort.h"

// #define DEBUG 1

int usage();

int main(int argc, char *argv[]) {
  int arr_size;
  if (argc != 2) { return usage(); }

  if (sscanf(argv[1], "%i", &arr_size) != 1) {
    printf("\nInvalid array size: %s\n", argv[1]);
    return usage();
  }

  int arr[arr_size];
  int i;

  // init array with worst case for sorting
  for (i=0 ; i < arr_size; i++)
    arr[i] = arr_size-i;

#ifdef DEBUG
  printf("\nBefore: ");
  for (i=0 ; i < arr_size; i++)
    printf("%03d ", arr[i]);
#endif

  bubble_sort(arr, arr_size);

#ifdef DEBUG
  printf("\nAfter:  ");
  for (i=0 ; i < arr_size; i++)
    printf("%03d ", arr[i]);
  printf("\n");
#endif

  return 0;
}

int usage() {
  printf("single-process ARRAY_SIZE\n");
  return 1;
}
