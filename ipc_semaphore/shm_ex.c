#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<string.h>
#include<semaphore.h>

int main(){
    pid_t pid;
    key_t key = 123231;
    char *shm,*temp,str[20];
    int shmid,i;
    sem_t mutex;        //semaphore

    if(sem_init(&mutex,1,1) == -1)  //semaphore initialisation
        perror("semaphore init failed\n");

    if((pid = fork()) == -1)        //fork new process
        perror("fork failed\n");

    if(pid == 0){       //child process
        if((key = ftok("/home/niwin/ESD/2nd_sem/rtos/ipc/shared_mem",'a')) == -1)  //make a unique key using ftok
            perror("ftok failed\n ");
        if((shmid = shmget(key,20,0666 | IPC_CREAT)) == -1)         //create/get shared memory using the key
            perror("shmget failed\n");
        if((shm = shmat(shmid,NULL,0)) == (char *)-1)       //point to the shared memory
            perror("shmat failed\n");
        printf("enter the string to be shared on shmid %d\n",shmid);
        scanf("%[^\n]s",str);
        temp = shm;
        for(i=0;i < strlen(str);i++){       //write to the shared memory
            sem_wait(&mutex);               //lock mutex(semaphore)
            *temp++=str[i];                 //access shared memory
            sem_post(&mutex);               //release mutex(semaphore)
            }
        sem_wait(&mutex);
        *temp = str[i];
        sem_post(&mutex);
        exit(1);
        }
    else{
        wait(NULL);
        if((key = ftok("/home/niwin/ESD/2nd_sem/rtos/ipc/shared_mem",'a')) == -1)       //generate the same key used to create shared memory
            perror("ftok failed\n ");
        if((shmid = shmget(key,20,0666)) == -1)         //get the shared memory id
            perror("shmget failed\n");
        if((shm = shmat(shmid,NULL,0)) == (char *)-1)   //point to the shared memory
            perror("shmat failed\n");
        printf("string shared by shmid %d\n",shmid);
        temp = shm;
        for(i=0;;i++){
            sem_wait(&mutex);       //lock the semaphore
            if(*temp == 0)          //access the shared memory
                break;
            str[i] = *temp++;
            sem_post(&mutex);       //release the semaphore
        }
        str[i] = '\n';
        for(i=0;str[i] != '\n';i++)
            printf("%c",str[i]);
        printf("\n");
    }

}
