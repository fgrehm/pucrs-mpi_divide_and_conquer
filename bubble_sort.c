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
