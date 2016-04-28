#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mpi.h"
#include "bubble_sort.h"

#define MASTER 0

#define TAG_JOB_CHUNK_INDEX      1
#define TAG_JOB_CHUNK_SIZE       1
#define TAG_JOB_CHUNK_PAYLOAD    3
#define TAG_RESULT_CHUNK_INDEX   4
#define TAG_RESULT_CHUNK_SIZE    5
#define TAG_RESULT_CHUNK_PAYLOAD 6

#define DEBUG 1

int arr_size, my_rank, proc_n;
int *arr;

void divide_or_conquer();
int usage();
void my_log(char *fmt, ...);

int main(int argc, char *argv[]) {
  if (argc != 2) { return usage(); }
  if (sscanf(argv[1], "%i", &arr_size) != 1) {
    printf("\nInvalid array size: %s\n", argv[1]);
    return usage();
  }

  int i;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

  if (my_rank == 0) {
    arr = calloc(arr_size, sizeof(int));
    // init array with worst case for sorting
    for (i = 0 ; i < arr_size; i++)
      arr[i] = arr_size-i;

#ifdef DEBUG
    printf("\nBefore: ");
    for (i = 0; i < arr_size; i++)
      printf("%03d ", arr[i]);
#endif
  }

  divide_or_conquer();

#ifdef DEBUG
  printf("\nAfter:  ");
  for (i = 0; i < arr_size; i++)
    printf("%03d ", arr[i]);
#endif

  MPI_Finalize();

  return 0;
}

void divide_or_conquer() {
  if (proc_n != 1) {
    printf("NOT SUPPORTED YET");
    return;
  }

  // if (arr_size <= )

  bubble_sort(arr, arr_size);
  // MPI_Status status;
  //  if (my_rank != 0) {
  //    sprintf(message, "Greetings from process %d!", my_rank);
  //    MPI_Send(message, strlen(message)+1, MPI_CHAR, MASTER, TAG_JOB_CHUNK_PAYLOAD, MPI_COMM_WORLD);
  //  } else {
  //    for (source = 1; source < proc_n; source++)
  //    {
  //      MPI_Recv(message, 100, MPI_CHAR , source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  //      printf("%s\n", message);
  //    }
  //  }
  //  MPI_Finalize();
}

int usage() {
  printf("divide-and-conquer ARRAY_SIZE\n");
  return 1;
}

void my_log(char *fmt, ...) {
  va_list printfargs;
  printf("[%d] ", my_rank);

  va_start(printfargs, fmt);
  vprintf(fmt, printfargs);
  va_end(printfargs);

  printf("\n");
}
