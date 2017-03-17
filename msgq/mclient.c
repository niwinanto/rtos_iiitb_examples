#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<unistd.h>

struct msg_queue{
    long mtype;
    int a,b;
    char op;
    pid_t cpid;
};

int main(){
    key_t key;
    int msqid,size;
    struct msg_queue buf;
    buf.mtype = 1;
    buf.cpid = getpid();                    //getting pid
    printf("enter the numbers and operant:ex-1 + 2\n");
    scanf("%d %c %d",&buf.a,&buf.op,&buf.b);
    size = sizeof(buf);

    if((key = ftok("/home/niwin",'a')) == -1)   //get unique key
        perror("ftok failed\n");

    if((msqid = msgget(key,0666 | IPC_CREAT)) == -1)  //create/open message queue and get msgqid
        perror("msgget failed\n");

    if(msgsnd(msqid,&buf,size,0) == -1)         //send data to server/message queue
          perror("msgsend failed\n");
    return 0;
}
