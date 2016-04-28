#include <stdio.h>
#include <stdlib.h>
#include "bubble_sort.h"

#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 40      // trabalho final com o valores 10.000, 100.000, 1.000.000

int main() {
    int vetor[ARRAY_SIZE];
    int i;

    // init array with worst case for sorting
    for (i=0 ; i<ARRAY_SIZE; i++)
      vetor[i] = ARRAY_SIZE-i;

#ifdef DEBUG
    printf("\nBefore: ");
    for (i=0 ; i<ARRAY_SIZE; i++)
      printf("%03d ", vetor[i]);
#endif

    bubble_sort(vetor, ARRAY_SIZE);

#ifdef DEBUG
    printf("\nAfter:  ");
    for (i=0 ; i<ARRAY_SIZE; i++)
      printf("%03d ", vetor[i]);
    printf("\n");
#endif

    return 0;
}
