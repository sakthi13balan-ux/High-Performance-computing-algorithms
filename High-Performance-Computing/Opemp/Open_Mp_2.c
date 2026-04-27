#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include<time.h>

int main() {
    int n,n_threads;
    double constant;
    printf("Enter the number of result:");
    scanf("%d",&n);
    printf("\nEmter the number of threads:");
    scanf("%d",&n_threads);
    printf("\nEnter the double constant:");
    scanf("%lf",&constant);
    srand(time(NULL));
    FILE *fp;

    double *arr = (double*)malloc(n * sizeof(double));
    int i;
    for ( i = 0; i < n; i++)
    {
        arr[i] = rand()%100;
    }

    #pragma omp parallel num_threads(n_threads)
    {
        int tid=omp_get_thread_num();
        double start_time=omp_get_wtime();
        #pragma omp for
        for (i = 0; i < n; i++) {
            arr[i] += constant;
        }
        double end_time=omp_get_wtime();
        printf("thread %d time taken:%f seconds\n",tid,end_time-start_time);
    }

    fp = fopen("result1.txt", "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    for (i = 0; i < n; i++) {
        fprintf(fp, "Index %d: %.1f\n", i, arr[i]);
    }
    fclose(fp);

    printf("First 10 results:\n");
    for (i = 0; i < 10; i++) {
        printf("Result[%d]: %.1f\n", i, arr[i]);
    }

    printf("\nAll results have been saved to 'result.txt'.\n");

    free(arr);
    return 0;
}