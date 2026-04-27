#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BUFFER_SIZE 6
#define MAX_WORD_LEN 50
#define RESULT_SIZE 100

char buffer[BUFFER_SIZE][MAX_WORD_LEN];
char result_buffer[RESULT_SIZE][MAX_WORD_LEN + 20];
int count = 0, result_count = 0;
int in = 0, out = 0;
int finished = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_not_empty = PTHREAD_COND_INITIALIZER;

const char *dictionary[] = {"HIGH_PERFORMANCE_COMPUTING","COMPUTATIONAL_INTELLIGENCE","CRYPTO","FSP","ETHICAL_HACKING","PROGRAMMING_WITH_JULIA","BIG_DATA"};
int dict_size = 7;

int is_in_dictionary(char *word)
{
    for (int i = 0; i < dict_size; i++)
    {
        if (strcmp(word, dictionary[i]) == 0) return 1;
    }
    return 0;
}

void* spell_check_worker(void* arg)
{
    while (1)
    {
        char word[MAX_WORD_LEN];

        pthread_mutex_lock(&lock);
        while (count == 0 && !finished)
        {
            pthread_cond_wait(&cond_not_empty, &lock);
        }

        if (count == 0 && finished)
        {
            pthread_mutex_unlock(&lock);
            break;
        }

        strcpy(word, buffer[in]);
        in = (in + 1) % BUFFER_SIZE;
        count--;

        pthread_cond_signal(&cond_not_full);
        pthread_mutex_unlock(&lock);

        char res_entry[MAX_WORD_LEN + 20];
        if (is_in_dictionary(word))
            sprintf(res_entry, "%s --> Correct", word);
        else
            sprintf(res_entry, "%s --> Incorrect", word);

        pthread_mutex_lock(&lock);
        if (result_count < RESULT_SIZE)
        {
            strcpy(result_buffer[result_count++], res_entry);
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t workers[3];
    int num_inputs;

    printf("Number of words would you like to check ?: ");
    if (scanf("%d", &num_inputs) != 1) return 1;

    for (int i = 0; i < 3; i++)
    {
        pthread_create(&workers[i], NULL, spell_check_worker, NULL);
    }

    for (int i = 0; i < num_inputs; i++)
    {
        char user_word[MAX_WORD_LEN];
        printf("Enter the name for spell check %d: ", i + 1);
        scanf("%s", user_word);

        pthread_mutex_lock(&lock);
        while (count == BUFFER_SIZE)
        {
            pthread_cond_wait(&cond_not_full, &lock);
        }

        strcpy(buffer[out], user_word);
        out = (out + 1) % BUFFER_SIZE;
        count++;

        pthread_cond_signal(&cond_not_empty);
        pthread_mutex_unlock(&lock);
    }

    pthread_mutex_lock(&lock);
    finished = 1;
    pthread_cond_broadcast(&cond_not_empty);
    pthread_mutex_unlock(&lock);

   for (int i = 0; i < 3; i++)
   {
        pthread_join(workers[i], NULL);
   }

    printf("\n--- Final Result Buffer ---\n");
    for (int i = 0; i < result_count; i++) {
        printf("%s\n", result_buffer[i]);
    }

    return 0;
}