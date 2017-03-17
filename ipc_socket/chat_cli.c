#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include<signal.h>

int sockfd = 0;
void nw_handler(int signo){          //signal handler
    char a[2];
    if(signo == SIGINT){             //check for ctl + c signal
        printf("pogram received a ctrl+c");
        printf("Are you sure[y/n]");
        scanf("%c",a);
        if(!strcmp("y",a)){
            close(sockfd);           //close socket in case of yes
                exit(0);
        }
        else
            printf("continuing");
    }
}

int main(int argc, char *argv[])    //usage: <./name><ip><port no>
{
    int  n = 0,i;
    char recvBuff[1024],sendBuff[1025],s;
    struct sockaddr_in serv_addr;

    if(argc != 3)                   //check for correct usage/arguments
    {
        printf("\n Usage: %s <ip of server> <port number> \n",argv[0]);
        return 1;

    }

    if(signal(SIGINT,nw_handler) == SIG_ERR)    //signal handler callback functio
        printf("cant acces SIDINT");

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)      //create socket
    {
        printf("\n Error : Could not create socket \n");
        return 1;

    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;         //set socket IPv4 protocol
    serv_addr.sin_port = htons(atoi(argv[2]));//set port no

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) //give settings,ip and port no to the opened socket struct
    {
        printf("\n inet_pton error occured\n");
        return 1;

    }

        if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) //connect to the server
        {
            printf("\n Error : Connect Failed \n");
            return 1;

        }
    while(1){
        n = read(sockfd, recvBuff, sizeof(recvBuff)); //read the socket
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
            printf("\n******** client terminated *********"); //if client terminate detect and terminate
            close(sockfd);
            exit(1);
        }
        printf("\n");
            printf("message:\n");
            fgets(sendBuff,100,stdin);
            //sendBuff[strlen(sendBuff)]='\n';
            write(sockfd, sendBuff, strlen(sendBuff)+1);        //write to the client socket

    }

    return 0;

}
