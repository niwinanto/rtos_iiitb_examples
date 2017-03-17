#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/sem.h>
#include<errno.h>

#define MAX_RETRIES 10
union semun{
    int val;
    struct semid_ds *buf;
    ushort *array;
};

int initsem(key_t key, int nsems){
    int i;
    union semun arg;
    struct semid_ds buf;
    struct sembuf sb;
    int semid,e;

    if((semid = semget(key,nsems,IPC_CREAT | IPC_EXCL | 0666)) == -1) //get/create semaphore
        perror("semget failed\n");

    if(semid >= 0){
        sb.sem_op = 1;
        sb.sem_flg = 0;
        arg.val = 1;

        for(sb.sem_num = 0;sb.sem_num < nsems;sb.sem_num++){
            /* do a semop() to "free" the semaphores. */
            /* this sets the sem_otime field, as needed below. */
            if(semop(semid,&sb,1)==-1){
                e = errno;
                semctl(semid, 0, IPC_RMID);
                errno = e;
                return -1;
            }
        }
    }
    else if(errno == EEXIST){
        /* someone else got it first */
        int ready = 0;
        semid = semget(key,nsems,0);
        if(semid < 0)
            return semid;
        /* wait for other process to initialize the semaphore: */
        arg.buf = &buf;
        for(i=0;i<MAX_RETRIES && !ready;i++){
            semctl(semid,nsems-1,IPC_STAT,arg);
            if(arg.buf->sem_otime !=0)
                ready = 1;
            else
                sleep(1);
        }
        if(!ready){
            errno = ETIME;
            return -1;
        }
    }
    else{
        return semid;
    }
    return semid;
}

int main(){
    pid_t pid;
    key_t key1=112233,key = 123231;
    char *shm,*temp,str[20];
    int shmid,i;
    int semid;
    struct sembuf sb;

    sb.sem_num = 0;
    sb.sem_op = -1;  /* set to allocate resource */
    sb.sem_flg = SEM_UNDO;

    if ((key1 = ftok("/home/niwin/ESD", 'J')) == -1) {
        perror("ftok");
        exit(1);

    }
    /* grab the semaphore set created by seminit.c: */
    if ((semid = initsem(key1, 1)) == -1) {
        perror("initsem");
        exit(1);

    }


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
            sb.sem_op = -1; /* set to allocate resource */
            if (semop(semid, &sb, 1) == -1) {
                perror("semop");
                exit(1);
            }
            //sem_wait(&mutex);               //lock mutex(semaphore)
            *temp++=str[i];                 //access shared memory
            //sem_post(&mutex);               //release mutex(semaphore)
            sb.sem_op = 1; /* free  resource */
            if (semop(semid, &sb, 1) == -1) {
                perror("semop");
                exit(1);
            }
        }
        //sem_wait(&mutex);

        sb.sem_op = -1; /* set to allocate resource */
        if (semop(semid, &sb, 1) == -1) {
            perror("semop");
            exit(1);
        }
        *temp = str[i];

        sb.sem_op = 1; /* free resource */
        if (semop(semid, &sb, 1) == -1) {
            perror("semop");
            exit(1);
        }
        //sem_post(&mutex);
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
            //sem_wait(&mutex);       //lock the semaphore
            sb.sem_op = -1; /* set to allocate resource */
            if (semop(semid, &sb, 1) == -1) {
                perror("semop");
                exit(1);
            }
            if(*temp == 0)          //access the shared memory
                break;
            str[i] = *temp++;
            //sem_post(&mutex);       //release the semaphore
            sb.sem_op = 1; /* free resource */
            if (semop(semid, &sb, 1) == -1) {
                perror("semop");
                exit(1);
            }
        }
        str[i] = '\n';
        for(i=0;str[i] != '\n';i++)
            printf("%c",str[i]);
        printf("\n");
    }

}
