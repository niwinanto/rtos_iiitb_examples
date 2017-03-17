#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

struct msg_queue{
    long mtype;
    int a,b;
    char op;
    pid_t cpid;
};

int main(int argc,char *argv[]){
    struct msqid_ds state;
    key_t key;
    int msqid,i=0,size,a,b,op,res;
    struct msg_queue buf;
    FILE *fp;
    size = sizeof(buf);

    if((key = ftok("/home/niwin",'a')) == -1)   //get the key used in client
        perror("ftok failed\n");

    if((msqid = msgget(key,0666 | IPC_CREAT)) == -1)  //connect to the client messagew queue
        perror("msgget failed\n");

    while(1){
        if(msgrcv(msqid,&buf,size,1,0) == -1)   //get data for message queue
            perror("msgreceive failed\n");

        if(buf.op == '+')               //mathematical operations
            res =buf.a+buf.b;
        else if(buf.op == '-')
            res = buf.a-buf.b;
        else if(buf.op == '/')
            res = buf.a/buf.b;
        else if(buf.op == '*')
            res = buf.a*buf.b;
        else{
            printf("not a valid operation\n");
            exit(0);
        }

        printf("caller pid:%d\noperation: %d%c%d = %d\n",buf.cpid,buf.a,buf.op,buf.b,res);
        fp = fopen(argv[1],"a+");
        fprintf(fp,"caller pid:%d\noperation: %d%c%d = %d\n",buf.cpid,buf.a,buf.op,buf.b,res);
        fclose(fp);
    }
}
