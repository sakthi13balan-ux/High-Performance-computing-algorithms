#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<mpi.h>
#include<ctype.h>
#include<time.h>
#define MAX 100

void toUpperCase(char str[])
{
        for(int i=0;str[i];i++)
                str[i]=toupper(str[i]);
}
int isPalindrome(char str[])
{
        int i=0,j=strlen(str)-1;
        while(i<j)
        {
                if(str[i]!=str[j])
                        return 0;
                i++;
                j--;
        }
        return 1;
}

int main()
{
        char msg[MAX];
        int comm_Size,my_Rank;
        const char  *words[]={"madam","malayalam","hi","HELLO","MPI","First"};
        int flag,count=6;
        struct timespec start,end;
        double exe_time;
        MPI_Status status;
        MPI_Init(NULL,NULL);
        MPI_Comm_size(MPI_COMM_WORLD,&comm_Size);
        MPI_Comm_rank(MPI_COMM_WORLD,&my_Rank);

        srand(time(NULL)+my_Rank);

        if(my_Rank!=0)
        {
                int idx=rand()%count;
                strcpy(msg,words[idx]);
                if(my_Rank%2==0)
                        MPI_Send(msg,strlen(msg)+1,MPI_CHAR,0,2,MPI_COMM_WORLD);
                else
                        MPI_Send(msg,strlen(msg)+1,MPI_CHAR,0,1,MPI_COMM_WORLD);
        }
        else
        {
                printf("\nMaster Process\n");
                for(int dest=1;dest<comm_Size;dest++)
                {
                        MPI_Recv(msg,MAX,MPI_CHAR,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
                        int sender=status.MPI_SOURCE;
                        int tag=status.MPI_TAG;
                        if(tag==1)
                        {
                                clock_gettime(CLOCK_MONOTONIC,&start);
                                toUpperCase(msg);
                                clock_gettime(CLOCK_MONOTONIC,&end);
                                exe_time=((end.tv_sec - start.tv_sec)*1000.0)+(end.tv_nsec - start.tv_nsec) / 1000000.0;
                                printf("\nMessage from process %d -> %s\nExecution time: %.6f milliseconds\n",sender,msg,exe_time);
                        }
                        else if(tag==2)
                        {
                                clock_gettime(CLOCK_MONOTONIC,&start);
                                flag=isPalindrome(msg);
                                clock_gettime(CLOCK_MONOTONIC,&end);
                                exe_time=((end.tv_sec - start.tv_sec)*1000.0)+(end.tv_nsec - start.tv_nsec) / 1000000.0;

                                if(flag)
                                        printf("\nMessage from process %d -> %s is a palindrome\nExecution time: %.6f milliseconds\n",sender,msg,exe_time);
                                else
                                        printf("\nMessage from process %d -> %s is not a palindrome\nExecution time: %.6f milliseconds\n",sender,msg,exe_time);

                        }
                }
        }
        MPI_Finalize();
        return 0;
}

