#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX_QUEUE 100
#define MAX_LINE 256

int NUM_PRODUCERS;
int NUM_CONSUMERS;

char queue[MAX_QUEUE][MAX_LINE];
int front = 0, rear = 0, count = 0;

int done_producers = 0;

omp_lock_t lock;

void print_queue_status()
{
    printf("Queue Status -> Front: %d | Rear: %d | Count: %d\n", front, rear, count);
}

void enqueue(char *line, int pid)
{
    while (1)
    {
        omp_set_lock(&lock);

        if (count < MAX_QUEUE)
        {
            printf("\n[ENQUEUE START] Producer %d\n", pid);

            strcpy(queue[rear], line);
            printf("Inserted at position %d: %s", rear, line);

            rear = (rear + 1) % MAX_QUEUE;
            count++;

            print_queue_status();

            printf("[ENQUEUE END]\n");

            omp_unset_lock(&lock);
            break;
        }

        omp_unset_lock(&lock);
    }
}

int dequeue(char *line, int cid)
{
    int got = 0;

    omp_set_lock(&lock);

    if (count > 0)
    {
        printf("\n[DEQUEUE START] Consumer %d\n", cid);

        strcpy(line, queue[front]);
        printf("Removed from position %d: %s", front, line);

        front = (front + 1) % MAX_QUEUE;
        count--;

        print_queue_status();

        printf("[DEQUEUE END]\n");

        got = 1;
    }

    omp_unset_lock(&lock);
    return got;
}

void producer(int id, char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        printf("Producer %d: Cannot open %s\n", id, filename);
        return;
    }

    char line[MAX_LINE];

    while (fgets(line, MAX_LINE, fp))
    {
        printf("\nProducer %d READ line: %s", id, line);
        enqueue(line, id);
    }

    fclose(fp);

    #pragma omp atomic
    done_producers++;

    printf("\nProducer %d finished. Total finished producers = %d\n", id, done_producers);
}

void consumer(int id)
{
    char line[MAX_LINE];

    while (1)
    {
        if (dequeue(line, id))
        {
            printf("Consumer %d processing line...\n", id);

            char *token = strtok(line, " \t\n");

            while (token != NULL)
            {
                printf("Consumer %d TOKEN: %s\n", id, token);
                token = strtok(NULL, " \t\n");
            }
        }
        else
        {
            #pragma omp flush(done_producers)

            if (done_producers == NUM_PRODUCERS)
            {
                printf("\nConsumer %d exiting (All producers done)\n", id);
                break;
            }
        }
    }
}

int main()
{
    printf("\nEnter number of producers: ");
    scanf("%d", &NUM_PRODUCERS);

    printf("Enter number of consumers: ");
    scanf("%d", &NUM_CONSUMERS);

    char *files[3] = {"file1.txt", "file2.txt", "file3.txt"};

    omp_init_lock(&lock);

    #pragma omp parallel num_threads(NUM_PRODUCERS + NUM_CONSUMERS)
    {
        int tid = omp_get_thread_num();

        if (tid < NUM_PRODUCERS)
        {
            producer(tid, files[tid]);
        }
        else
        {
            consumer(tid - NUM_PRODUCERS);
        }
    }

    omp_destroy_lock(&lock);

    return 0;
}