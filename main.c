#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omp.h"

#define threshold 1000

void swap(int* a, int* b)
{
	int t = *a;
	*a = *b;
	*b = t;
}

void partition(int *v, int low, int high, int *left, int *right)
{
    int i = low;
    int j = high;
    int pivot = v[(i + j) / 2];

    while (i <= j) {
        while (v[i] < pivot) i++;
        while (v[j] > pivot) j--;

        if (i <= j) {
            swap(&v[i], &v[j]);
            i++;
            j--;
        }
    }

    *left = i;
    *right = j;
}

void quicksort_tasks(int *v, int low, int high)
{
    int left = 0;
    int right = 0;
    partition(v, low, high, &left, &right);

    if (high - low < threshold || right - low < threshold || high - left < threshold) {
        if (low < right)
            quicksort_tasks(v, low, right);
        if (left < high)
            quicksort_tasks(v, left, high);
    } else {
        #pragma omp task
        { quicksort_tasks(v, low, right); }
        quicksort_tasks(v, left, high);
    }
}

int main(int argc, char **argv)
{
    int size = (argc > 1 && argv[1] != NULL) ? atoi(argv[1]) : 10;
    int threads = (argc > 2 && argv[2] != NULL) ? atoi(argv[2]) : 4;
    omp_set_num_threads(threads);
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

    printf("# qsort (size = %d, threads = %d)\n", size, threads);
	printf("# Elapsed time (sec): %.5f\n", t);
    return 0;
}
