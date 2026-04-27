#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define mod 10

int main() {
    int my_Rank, comm_Sz;
    int n;
    int *arr = NULL;
    int *local_arr;
    int local_n;
    struct timespec start,end;
    double exe_time;

    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_Rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_Sz);


    if (my_Rank == 0) {
        while (1) {
            printf("Enter number of elements (n): \n");
            scanf("%d", &n);

            if (n % comm_Sz == 0)
                break;

            printf("n must be divisible by number of processes (%d). Try again.\n",comm_Sz);
        }

        arr = (int *)malloc(n * sizeof(int));

        srand(time(NULL));
        printf("\nGenerated Array:\n");
        for (int i = 0; i < n; i++) {
            arr[i] = rand() % mod;
            printf("%d ", arr[i]);
        }
        printf("\n\n");
        local_n = n /comm_Sz;
    }

    MPI_Bcast(&local_n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    local_arr = (int *)malloc(local_n * sizeof(int));
    MPI_Scatter(arr, local_n, MPI_INT,local_arr, local_n, MPI_INT,0, MPI_COMM_WORLD);

    int *local_freq = (int *)calloc(mod, sizeof(int));
    int *global_freq = NULL;

    if (my_Rank == 0)
        global_freq = (int *)calloc(mod, sizeof(int));

    clock_gettime(CLOCK_MONOTONIC,&start);
    for (int i = 0; i < local_n; i++)
        local_freq[local_arr[i]]++;
    clock_gettime(CLOCK_MONOTONIC,&end);
    exe_time=((end.tv_sec - start.tv_sec)*1000.0)+(end.tv_nsec - start.tv_nsec) / 1000000.0;

    printf("\nProcess %d computed frequency for the following :\n",my_Rank);
    printf("\n");
    for(int i=0;i<local_n;i++)
    {
            printf("%d-->%d\t",local_arr[i],local_freq[local_arr[i]]);
            printf("\n");
    }
    printf("\nExecution time: %.6f milliseconds\n",exe_time);
    MPI_Reduce(local_freq, global_freq, mod, MPI_INT,MPI_SUM, 0, MPI_COMM_WORLD);


    if (my_Rank == 0)
    {
        printf("\nFinal Frequency Count:\n");
        for (int i = 0; i < mod; i++)
        {
                if(global_freq[i]>0)
                        printf("Element %d -> %d times\n", i, global_freq[i]);
        }
        free(arr);
        free(global_freq);
    }

    free(local_arr);
    free(local_freq);

    MPI_Finalize();
    return 0;
}