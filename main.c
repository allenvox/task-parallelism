#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omp.h"

#define THRESHOLD 1000

void swap(int* a, int* b)
{
	int t = *a;
	*a = *b;
	*b = t;
}

void partition(int *v, int i, int j, int low, int high)
{
    i = low;
    j = high;
    int pivot = v[(low + high) / 2];
    do {
        while (v[i] < pivot) i++;
        while (v[j] > pivot) j--;
        if (i <= j) {
            swap(&v[i], &v[j]);
            i++;
            j--;
        }
    } while (i <= j);
}

void quicksort_tasks(int *v, int low, int high)
{
    int i, j;
    partition(v, i, j, low, high);
    if (high - low < THRESHOLD || (j - low < THRESHOLD || high - i < THRESHOLD)) {
        if (low < j)
            quicksort_tasks(v, low, j);
        if (i < high)
            quicksort_tasks(v, i, high);
    } else {
        #pragma omp task
        { quicksort_tasks(v, low, j); }
        quicksort_tasks(v, i, high);
    }
}

int main(int argc, char **argv)
{
    int size = (argc > 1 && argv[1] != NULL) ? atoi(argv[1]) : 10;
    int *array = malloc(size * sizeof(int));

    //printf("unsorted: ");
    for (int i = 0; i < size; i++) {
       array[i] = rand() % size + 1;
       //printf("%d ", array[i]);
    }
    
    double t = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single nowait
        quicksort_tasks(array, 0, size - 1);
    }
    t = omp_get_wtime() - t;

    //printf("sorted: ");
    for (int i = 0; i < size; i++) {
       //printf("%d ", array[i]);
    }

    printf("# qsort (size = %d)\n", size);
	printf("# Elapsed time (sec): %.5f\n", t);
    return 0;
}