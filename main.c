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
    if ((argc != 2) || atoi(argv[1]) == 0) {
        printf("Usage: ./main <positive_array_size>\n");
        return 0;
    }
    int size = atoi(argv[1]);
    int *array = malloc(sizeof(int) * size);

    //printf("unsorted: ");
    for (int i = 0; i < size; i++) {
       array[i] = rand() % size + 1;
       //printf("%d ", array[i]);
    }

    double t = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
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
