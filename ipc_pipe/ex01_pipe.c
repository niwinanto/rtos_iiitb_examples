#include<stdio.h>
#include<unistd.h>
#include<string.h>

int main(){
    pid_t prsid;
    int pipfd[2];
    char rdbfr[10];
    char data[]="niwin";
    pipe(pipfd);
    prsid=fork();
    if(prsid == -1)
        perror("fork not worked\n");
    if(prsid == 0){
        //child process
        close(pipfd[1]);
        printf("child reading\n");
        if(read(pipfd[0],rdbfr,sizeof(rdbfr)) == -1)
            perror("pip read failed at child\n");
        else
            printf("received string is %s \n",rdbfr);
        close(pipfd[0]);
    }
    else{
        //parent process
        close(pipfd[0]);
        printf("parent writing\n");
        if(write(pipfd[1],data,strlen(data)+1) == -1)
            perror("parent writing failed\n");
        close(pipfd[1]);
    }
}
