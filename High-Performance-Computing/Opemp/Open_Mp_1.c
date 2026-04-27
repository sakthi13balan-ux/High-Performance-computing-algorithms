#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
int main()
{
    int n,n_threads;
    printf("Enter the number of result:");
    scanf("%d",&n);
    printf("\nEnter the number of threads:");
    scanf("%d",&n_threads);
    FILE *fp;
    srand(time(NULL));
    double *a = (double*)malloc(n * sizeof(double));
    double *b = (double*)malloc(n * sizeof(double));
    double *c = (double*)malloc(n * sizeof(double));
    int i;
    for (i = 0; i < n; i++) {
        a[i] = rand()%100;
        b[i] = rand()%100;
    }

    fp = fopen("out.txt", "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    #pragma omp parallel num_threads(n_threads)
    {
       int tid=omp_get_thread_num();
        double start_time=omp_get_wtime();
        #pragma omp for
        for (i = 0; i < n; i++) {
                c[i] = a[i] + b[i];
        }
        double end_time=omp_get_wtime();
        printf("Thread %d time taken:%f seconds\n",tid,end_time-start_time);
    }

   for (i = 0; i < n; i++) {
        fprintf(fp, "Result: C[%d] : %.1f\n", i, c[i]);
    }

     for(i=0;i<10;i++)
     {
        printf("Result: C[%d] : %.1f\n", i, c[i]);
     }

    fclose(fp);

    printf("Results successfully written to out.txt\n");

    free(a);
    free(b);
    free(c);

    return 0;
}