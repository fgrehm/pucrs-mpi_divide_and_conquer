#include <stdlib.h>

void bubble_sort(int *arr, int arr_size) {
  int i = 0, j, aux, replaced = 1;

  while (i < (arr_size - 1) && replaced) {
    replaced = 0;
    for (j = 0 ; j < arr_size - i - 1; j++) {
      if (arr[j] > arr[j+1]) {
        aux      = arr[j];
        arr[j]   = arr[j+1];
        arr[j+1] = aux;
        replaced = 1;
      }
    }
    i++;
  }
}

int *interleaving(int *arr, int arr_size) {
  int *aux_arr;
  int i1, i2, i_aux;

  aux_arr = (int *)malloc(sizeof(int) * arr_size);

  i1 = 0;
  i2 = arr_size / 2;

  for (i_aux = 0; i_aux < arr_size; i_aux++) {
    if (((arr[i1] <= arr[i2]) && (i1 < (arr_size / 2)))
        || (i2 == arr_size))
      aux_arr[i_aux] = arr[i1++];
    else
      aux_arr[i_aux] = arr[i2++];
  }

  return aux_arr;
}
