#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>

int main(){
    FILE *fp,*fl;
    int pid[2];
    pid_t  id;
    char s,rdbfr;

    if(pipe(pid) == -1)         //call pipe system call
        perror("pipe failed");

    if((id  = fork()) == -1)        //call fork system call and return process id
        perror("fork failed\n");

    if(id == 0){                    //child process starts
        fp = popen("ls -l *.jpg","r");    //open the file pointer to execute and read output of shell command
        close(pid[0]);              //close read end of pipe
        while(!feof(fp)){
            s = fgetc(fp);
            if(write(pid[1],&s,1) == -1)    //write the output of shell command to pipe write end
                perror("child writing failed");
        }
        close(pid[1]);              //close pipe write end
        fclose(fp);
        exit(1);
    }
    else{
        //wait(NULL);
        fl = popen("wc -l","w");  //open file pointer to write to second shell command
        close(pid[1]);              //close pipe write end
        while(rdbfr != EOF){
            if(read((pid[0]),&rdbfr,1) == -1)   //read from pipe read end
                perror("pip read failed at parent");
            else
                fprintf(fl,"%c",rdbfr); //write to the shell command file pointer
            //printf("%c\n",rdbfr);
        }
        close(pid[0]);              //close pipe read end
        fclose(fl);
    }
    return 0;
}
