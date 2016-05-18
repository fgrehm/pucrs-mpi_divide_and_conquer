#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mpi.h"
#include "bubble_sort.h"

#define ROOT 0

#define TAG_JOB_SIZE       1
#define TAG_JOB_PAYLOAD    2
#define TAG_RESULT_PAYLOAD 3

#define DEBUG 1

int my_rank, proc_n, tree_height, conquer_condition, parent, left_child, right_child, local_processing_percentage;

void divide_and_conquer(int *arr, int arr_size);
void conquer(int *arr, int arr_size);
void divide_or_conquer();
void send_job(int *arr, int arr_size, int dest);
void receive_job(int **arr, int* arr_size, int source);
void send_results(int *arr, int arr_size, int dest);
void receive_results(int *arr, int arr_size, int source);
void merge_arrays(int* arr, int arr_size, int offset, int* other_arr, int other_arr_size);

void debug_array(int *arr, int arr_size);
void my_log(char *fmt, ...);

int usage() {
  if (my_rank == ROOT)
    printf("divide-and-conquer ARRAY_SIZE [LOCAL_PROCESSING_PERCENTAGE]\n");
  return 1;
}

int main(int argc, char *argv[]) {
  int arr_size;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

  if (argc < 2 || argc > 3) {
    MPI_Finalize();
    return usage();
  }
  if (sscanf(argv[1], "%i", &arr_size) != 1) {
    MPI_Finalize();
    if (my_rank == ROOT) printf("\nInvalid array size: %s\n", argv[1]);
    return usage();
  }
  if (argc == 3) {
    if (sscanf(argv[2], "%i", &local_processing_percentage) != 1) {
      MPI_Finalize();
      if (my_rank == ROOT) printf("\nInvalid percentage: %s\n", argv[2]);
      return usage();
    }
  }

  tree_height = (int)log2(proc_n+1);
  conquer_condition = arr_size / pow(2, tree_height-1) + 1;
  parent = (int)(my_rank-1)/2;
  left_child = my_rank * 2 + 1;
  right_child = my_rank * 2 + 2;

#ifdef DEBUG
  if (my_rank == ROOT) {
    my_log("Tree height is %d, will conquer when array is reduced to <= %d elements for %d leaf nodes\n\n", tree_height, conquer_condition, (int)pow(2, tree_height-1));
  }
  my_log("parent=%d,left=%d,right=%d", parent, left_child, right_child);
#endif

  int *arr;
  int i;
  if (my_rank == ROOT) {
    arr = calloc(arr_size, sizeof(int));
    for (i = 0 ; i < arr_size; i++)
      arr[i] = arr_size-i;

#ifdef DEBUG
    printf("\nBefore: "); debug_array(arr, arr_size);
#endif

    divide_and_conquer(arr, arr_size);
  } else  {
    divide_or_conquer();
  }

#ifdef DEBUG
  if (my_rank == ROOT) {
    printf("\nAfter:  "); debug_array(arr, arr_size);
  }
#endif

  MPI_Finalize();

  return 0;
}

void divide_and_conquer(int *arr, int arr_size) {
  my_log("Divide and conquer array of size %d", arr_size);

  int offset = arr_size * (local_processing_percentage / 100.0);
  int half_size = (arr_size - offset) / 2;

  send_job(arr + offset, half_size, left_child);
  send_job(arr + offset + half_size, half_size, right_child);

  if (offset > 0) {
    my_log("Ordering first %d elements locally", offset);
    bubble_sort(arr, offset);
  }

  my_log("Waiting for results from %d", left_child);
  receive_results(arr + offset, half_size, left_child);
  my_log("Waiting for results from %d", right_child);
  receive_results(arr + offset + half_size, half_size, right_child);

  my_log("Interleaving");
  int *new_arr = interleaving(arr + offset, arr_size - offset);

  if (offset == 0) {
    my_log("Copying");
    memcpy(arr, new_arr, arr_size * sizeof(int));
    free(new_arr);
  } else {
    my_log("Merging arrays");
    merge_arrays(arr, arr_size, offset, new_arr, arr_size - offset);
  }
  my_log("DONE");

  if (my_rank != ROOT) {
    send_results(arr, arr_size, parent);
  }
}

void divide_or_conquer() {
  int *arr, arr_size;

  receive_job(&arr, &arr_size, parent);
  if (left_child < proc_n)
    divide_and_conquer(arr, arr_size);
  else
    conquer(arr, arr_size);
}

void conquer(int *arr, int arr_size) {
  my_log("Conquer array of size %d", arr_size);
  bubble_sort(arr, arr_size);
  my_log("Sending back results to %d", parent);
  send_results(arr, arr_size, parent);
}

void send_job(int *arr, int arr_size, int dest) {
  my_log("Sending job with size %d to %d", arr_size, dest);

  MPI_Send(&arr_size, 1, MPI_INT, dest, TAG_JOB_SIZE, MPI_COMM_WORLD);
  MPI_Send(arr, arr_size, MPI_INT, dest, TAG_JOB_PAYLOAD, MPI_COMM_WORLD);
}

void receive_job(int **arr, int* arr_size, int source) {
  MPI_Status status;

  MPI_Recv(arr_size, 1, MPI_INT, source, TAG_JOB_SIZE, MPI_COMM_WORLD, &status);
  my_log("Received job with size %d", *arr_size);

  *arr = (int*)calloc(*arr_size, sizeof(int));
  MPI_Recv(*arr, *arr_size, MPI_INT, source, TAG_JOB_PAYLOAD, MPI_COMM_WORLD, &status);
}

void send_results(int *arr, int arr_size, int dest) {
  MPI_Send(arr, arr_size, MPI_INT, dest, TAG_RESULT_PAYLOAD, MPI_COMM_WORLD);
}

void receive_results(int *arr, int arr_size, int source) {
  MPI_Status status;
  MPI_Recv(arr, arr_size, MPI_INT, source, TAG_RESULT_PAYLOAD, MPI_COMM_WORLD, &status);
}

void merge_arrays(int* arr, int arr_size, int offset, int* other_arr, int other_arr_size) {
  int* aux_arr = calloc(offset, sizeof(int));
  memcpy(aux_arr, arr, offset * sizeof(int));

  int i1 = 0, i2 = 0, i_arr;
  for (i_arr = 0; i_arr < arr_size; i_arr++) {
    if (((i1 < offset) && (aux_arr[i1] <= other_arr[i2]))
        || (i2 == other_arr_size)) {
      arr[i_arr] = aux_arr[i1++];
    } else {
      arr[i_arr] = other_arr[i2++];
    }
  }

  free(aux_arr);
}

void debug_array(int *arr, int arr_size) {
  int i;
  for (i = 0; i < 11; i++)
    printf("%07d ", arr[i]);
  printf("... ");
  for (i = arr_size - 12; i < arr_size; i++)
    printf("%07d ", arr[i]);
  printf("\n");
}

void my_log(char *fmt, ...) {
  va_list printfargs;
  printf("[%d] ", my_rank);

  va_start(printfargs, fmt);
  vprintf(fmt, printfargs);
  va_end(printfargs);

  printf("\n");
}
