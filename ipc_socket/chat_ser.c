#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include<fcntl.h>
#include<signal.h>

int listenfd = 0,connfd = 0;

void nw_handler(int signo){         //signal handler
    char a[2];
    if(signo == SIGINT){
        printf("pogram received a ctrl+c");
        printf("Are you sure[y/n]");
        scanf("%c",a);
        if(!strcmp("y",a)){
            close(connfd);      //if continue with ctl + c close the client socket
            close(listenfd);    //close the server socket
                exit(0);
        }
        else
            printf("continuing");
    }
}

int main(int argc, char *argv[])    //usage is <./name> <port no>
{
    int n;
    struct sockaddr_in serv_addr;

    char sendBuff[1025],recvBuff[1024],s;

    if(argc != 2)           //check for proper command line arguments
    {
        printf("\n Usage: %s  <port number> \n",argv[0]);
        return 1;

    }
    if(signal(SIGINT,nw_handler) == SIG_ERR)    //signal handler call
        printf("cant acces SIDINT");

    listenfd = socket(AF_INET, SOCK_STREAM, 0);     //make the socket
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;                 //socket protocol and settings:IPv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));      //set port number

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));        //bind the socket with the socket settings struct

    listen(listenfd, 10);       //set the connection limit to 10
    while(1)
    {
        printf("waiting for connection\n");
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);//connect to the client
        printf("connected\n");
        while(1){

            printf("message:\n");
            fgets(sendBuff,100,stdin);
            //sendBuff[strlen(sendBuff)]='\n';
            write(connfd, sendBuff, strlen(sendBuff)+1);        //write to the client socket

        n = read(connfd, recvBuff, sizeof(recvBuff)); //read the socket
        printf("\nreceived message:\n");
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
        if(n < 0)
        {
            printf("\n Read error \n");
        }
        if(n == 0){
            printf("\n******** server terminated *********"); //if server terminate detect and terminate
            close(connfd);
            exit(1);
        }
        printf("\n");
            }
        close(connfd);
        sleep(1);
    }

}
