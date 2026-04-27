#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#define MAX 1000
#define PRINT_LIMIT 10

int A[MAX][MAX], B[MAX][MAX];
int Add[MAX][MAX], Sub[MAX][MAX];

double time_diff(struct timespec s, struct timespec e)
{
    return (e.tv_sec - s.tv_sec)*1000.0 +
           (e.tv_nsec - s.tv_nsec) / 1000000.0;
}

int main()
{
    int i,j,r, c;
    pid_t pid;
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe failed");
        return 1;
    }

    printf("Enter number of rows and columns: ");
    scanf("%d %d", &r, &c);

    if (r > MAX || c > MAX)
    {
        printf("Matrix size exceeds limit!\n");
        return 1;
    }

    srand(time(NULL));

    for ( i = 0; i < r; i++)
        for ( j = 0; j < c; j++)
        {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }

    pid = fork();

    if (pid == 0)
    {
        close(fd[0]);

        struct timespec cs, ce;
        clock_gettime(CLOCK_MONOTONIC, &cs);

        for (i = 0; i < r; i++)
            for ( j = 0; j < c; j++)
                Sub[i][j] = A[i][j] - B[i][j];

        clock_gettime(CLOCK_MONOTONIC, &ce);

        if (r <= PRINT_LIMIT && c <= PRINT_LIMIT)
        {
            printf("\nChild Process (PID %d): Matrix Subtraction\n", getpid());
            for ( i = 0; i < r; i++)
            {
                for ( j = 0; j < c; j++)
                    printf("%4d ", Sub[i][j]);
                printf("\n");
            }
        }
        double sub_time = time_diff(cs, ce);
        printf("\nChild (Subtraction) execution time: %.6f milliseconds\n",
               sub_time);

        write(fd[1], &sub_time, sizeof(sub_time));
        close(fd[1]);

        exit(0);
    }
    else if (pid > 0)
    {
        close(fd[1]);

        struct timespec ps, pe;
        clock_gettime(CLOCK_MONOTONIC, &ps);

        for ( i = 0; i < r; i++)
            for ( j = 0; j < c; j++)
                Add[i][j] = A[i][j] + B[i][j];

        clock_gettime(CLOCK_MONOTONIC, &pe);

        wait(NULL);

        if (r <= PRINT_LIMIT && c <= PRINT_LIMIT)
        {
            printf("\nParent Process (PID %d): Matrix Addition\n", getpid());
            for ( i = 0; i < r; i++)
            {
                for ( j = 0; j < c; j++)
                    printf("%4d ", Add[i][j]);
                printf("\n");
            }
        }
        double add_time = time_diff(ps, pe);
        printf("\nParent (Addition) execution time: %.6f milliseconds\n",
               add_time);

        double sub_time_from_child;
        read(fd[0], &sub_time_from_child, sizeof(sub_time_from_child));
        close(fd[0]);

        double total_time = (add_time > sub_time_from_child) ? add_time : sub_time_from_child;

        printf("Total execution time:%.6f milliseconds\n",total_time);
    } else {
        perror("fork failed");
        return 1;
    }

    return 0;
}