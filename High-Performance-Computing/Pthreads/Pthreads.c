#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 10

int A[MAX][MAX], B[MAX][MAX];
int sum[MAX][MAX], product[MAX][MAX];
int r1, c1, r2, c2;
struct timespec start,end;
double exe_time;
void* matrix_addition(void* arg)
{

    clock_gettime(CLOCK_MONOTONIC,&start);
    if (r1 != r2 || c1 != c2)
    {
        printf("\nMatrix Addition not possible (dimension mismatch)\n");
        pthread_exit(NULL);
    }

    for (int i = 0; i < r1; i++)
        for (int j = 0; j < c1; j++)
            sum[i][j] = A[i][j] + B[i][j];

    printf("\nMatrix Addition Result:\n");
    for (int i = 0; i < r1; i++)
    {
        for (int j = 0; j < c1; j++)
            printf("%d ", sum[i][j]);
        printf("\n");
    }
   clock_gettime(CLOCK_MONOTONIC,&end);
   exe_time=((end.tv_sec - start.tv_sec)*1000.0)+(end.tv_nsec - start.tv_nsec) / 1000000.0;
   printf("\nThread 1 Execution time: %.6f milliseconds\n",exe_time);

    pthread_exit(NULL);
}

void* matrix_multiplication(void* arg)
{

    clock_gettime(CLOCK_MONOTONIC,&start);
    if (c1 != r2)
    {
        printf("\nMatrix Multiplication not possible (dimension mismatch)\n");
        pthread_exit(NULL);
    }

    for (int i = 0; i < r1; i++)
    {
        for (int j = 0; j < c2; j++)
        {
            product[i][j] = 0;
            for (int k = 0; k < c1; k++)
                product[i][j] += A[i][k] * B[k][j];
        }
    }

    printf("\nMatrix Multiplication Result:\n");
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            printf("%d ", product[i][j]);
        }
        printf("\n");
    }

  clock_gettime(CLOCK_MONOTONIC,&end);
  exe_time=((end.tv_sec - start.tv_sec)*1000.0)+(end.tv_nsec - start.tv_nsec) / 1000000.0;
  printf("\nThread  2 Execution time: %.6f milliseconds\n",exe_time);

    pthread_exit(NULL);
}

int main() {
    pthread_t t1, t2;
    struct timespec start,end;
    double exec_time;

    printf("Enter rows and columns of Matrix A: ");
    scanf("%d %d", &r1, &c1);

    printf("Enter rows and columns of Matrix B: ");
    scanf("%d %d", &r2, &c2);

    printf("\nEnter elements of Matrix A:\n");
    for (int i = 0; i < r1; i++)
        for (int j = 0; j < c1; j++)
            scanf("%d", &A[i][j]);

    printf("\nEnter elements of Matrix B:\n");
    for (int i = 0; i < r2; i++)
        for (int j = 0; j < c2; j++)
            scanf("%d", &B[i][j]);

   pthread_create(&t1, NULL, matrix_addition,NULL);

   pthread_create(&t2, NULL, matrix_multiplication, NULL);

   pthread_join(t1, NULL);
   pthread_join(t2, NULL);

   printf("\nBoth threads finished execution.\n");

    return 0;
}