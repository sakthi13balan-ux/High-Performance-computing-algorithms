#include <stdio.h>
#include <omp.h>

double f(double x)
{
    return x * x;
}

int main()
{
    int n, n_threads;
    printf("\nEnter the number of trapezoids: ");
    scanf("%d", &n);
    double a, b;
    printf("\nEnter the a and b: ");
    scanf("%lf %lf", &a, &b);
    printf("\nEnter the number of threads: ");
    scanf("%d", &n_threads);
    double h, integral = 0.0;
    h = (b - a) / n;
    double thread_sum[n_threads];
    double thread_min[n_threads];
    double thread_max[n_threads];
    for (int i = 0; i < n_threads; i++)
    {
        thread_sum[i] = 0.0;
        thread_min[i] = 1e9;
        thread_max[i] = -1e9;
    }
    omp_set_num_threads(n_threads);
    #pragma omp parallel for reduction(+:integral)
    for (int i = 0; i < n; i++)
    {
        int tid = omp_get_thread_num();
        double x_i = a + i * h;
        double x_next = a + (i + 1) * h;
        double local_area = (h / 2.0) * (f(x_i) + f(x_next));
        printf("Thread %d -> Trapezoid %d: Area = %f\n",tid, i, local_area);
        integral += local_area;
        thread_sum[tid] += local_area;
        if (x_i < thread_min[tid])
            thread_min[tid] = x_i;
        if (x_next > thread_max[tid])
            thread_max[tid] = x_next;
    }

    printf("\n--- Per Thread Summary ---\n");
    for (int i = 0; i < n_threads; i++)
        if (thread_min[i] <= thread_max[i])
            printf("Thread %d: Total Area = %f, Left End = %f, Right End = %f\n",i, thread_sum[i], thread_min[i], thread_max[i]);

    printf("\nApproximate integral = %f\n", integral);
    return 0;
}