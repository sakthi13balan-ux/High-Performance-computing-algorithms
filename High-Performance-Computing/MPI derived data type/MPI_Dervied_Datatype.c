#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<mpi.h>

#define MAX 100
#define NAME_LEN 50

typedef struct
{
   char name[NAME_LEN];
   int rollno;
   float mark;
   char grade;
} Student;

char calculate_Grade(float mark)
{
    if (mark >= 90) return 'A';
    else if (mark >= 80) return 'B';
    else if (mark >= 70) return 'C';
    else if (mark >= 60) return 'D';
    else return 'E';
}

int main()
{
   int my_Rank, comm_Sz;

   MPI_Init(NULL, NULL);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_Sz);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_Rank);

   Student students[MAX];
   Student gathered[MAX];
   int total_Students = 0;
   double start_time,end_time,exec_time;

   MPI_Datatype MPI_STUDENT;
   int blocks[4] = {NAME_LEN, 1, 1, 1};
   MPI_Datatype types[4] = {MPI_CHAR, MPI_INT, MPI_FLOAT, MPI_CHAR};
   MPI_Aint disp[4], base;
   Student temp;

   MPI_Get_address(&temp, &base);
   MPI_Get_address(&temp.name, &disp[0]);
   MPI_Get_address(&temp.rollno, &disp[1]);
   MPI_Get_address(&temp.mark, &disp[2]);
   MPI_Get_address(&temp.grade, &disp[3]);

   for(int i = 0; i < 4; i++)
       disp[i] -= base;

   MPI_Type_create_struct(4, blocks, disp, types, &MPI_STUDENT);
   MPI_Type_commit(&MPI_STUDENT);

   if(my_Rank == 0)
   {
      FILE *fp = fopen("students.txt", "r");
      if(fp == NULL)
      {
         printf("Error opening file\n");
         MPI_Abort(MPI_COMM_WORLD, 1);
      }

      while(fscanf(fp, "%s %d %f",
             students[total_Students].name,
             &students[total_Students].rollno,
             &students[total_Students].mark) != EOF)
      {
         total_Students++;
      }
      fclose(fp);
   }

   MPI_Bcast(&total_Students, 1, MPI_INT, 0, MPI_COMM_WORLD);

   int local_Count = total_Students / comm_Sz;
   Student local_Student[local_Count];

   MPI_Scatter(students, local_Count, MPI_STUDENT,local_Student, local_Count, MPI_STUDENT,0, MPI_COMM_WORLD);

   start_time=MPI_Wtime();
   for(int i = 0; i < local_Count; i++)
   {
      local_Student[i].grade = calculate_Grade(local_Student[i].mark);
      printf("Process %d :\tName:%s \tRoll:%d\tMark:%f\tGrade Calculated:%c\n",my_Rank,local_Student[i].name,local_Student[i].rollno,local_Student[i].mark,local_Student[i].grade);
   }

   end_time=MPI_Wtime();
   exec_time=end_time-start_time;
   printf("Process %d execution time : %f milliseconds\n",my_Rank,exec_time*1000);

   MPI_Gather(local_Student, local_Count, MPI_STUDENT,gathered, local_Count, MPI_STUDENT,0, MPI_COMM_WORLD);

   if(my_Rank == 0)
   {
      FILE *out = fopen("grades.txt", "w");

      for(int i = 0; i < total_Students; i++)
         fprintf(out, "%s %d %.2f %c\n",
                 gathered[i].name,
                 gathered[i].rollno,
                 gathered[i].mark,
                 gathered[i].grade);

      fclose(out);
   }

   MPI_Type_free(&MPI_STUDENT);
   MPI_Finalize();
   return 0;
}