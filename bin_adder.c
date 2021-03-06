#include "myheaders.h"

void waitRandom();
void writeFile();

int GetSharedIDFromFile()
{
        FILE * fptr;
        int num = 0;
        fptr = fopen(sharedIDFile, "r");
        fscanf(fptr,"%d", &num);
        fclose(fptr);
        return num;
}

void writeFile(int size, int index, int total)
{
        FILE * fptr;
        if((fptr = fopen(outputFile, "ab+"))== NULL)
        {
                perror("file problems\n");
                exit(1);
        }
        else
        {
                fprintf(fptr, "Pid %d Index %d Size %d \n", getpid(), index, size);
                fprintf(stderr, "From Child: Pid %d Index %d Size %d \n", getpid(), index, size);
        }
        fclose(fptr);
}

void waitRandom()
{
        srand(time(0));
        int sleepTime = 0;
        sleepTime = rand() % 4;
        sleep(sleepTime);
}

int main(int argc, char ** argv)
{
        int index = 0;
        int size = 0;
        sscanf(argv[0], "%d", &index);
        sscanf(argv[1], "%d", &size);

        sharedID = GetSharedIDFromFile();
        arr = (int*) shmat(sharedID, NULL, 0);
        int total  = 0;

        for(int i = 0; i < size; i++)
        {
                total = total + arr[index + i +2];
        }
        arr[index + 2] = total;

        //shmdt(arr);
        sem_t* mutex = sem_open(semaphoreName, O_CREAT, 0666, 1000);
        if(mutex==NULL)
        {
                perror("sem_open()");
                exit(1);
        }
        //shmdt(arr);

        for(int i = 0; i < 5; i++)
        {
                waitRandom();
                if (sem_wait(mutex) == 0) {
                  sleep(1);
                  fprintf(stderr, "Pid %d is requesting to enter critical section at clock %d seconds and %d micro seconds  \n", getpid(), arr[0],arr[1]);
                  writeFile(size, index, total);
                  sleep(1);
                  sem_post(mutex);
                  break;
                }
        }

        sem_close(mutex);
        shmdt(arr);
        return 0;
}
