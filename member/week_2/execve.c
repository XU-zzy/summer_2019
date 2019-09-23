#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<wait.h>

int main(int argc,char *argv[],char **environ){
    pid_t pid;
    int stat_val;
    printf("exec example!\n");

    pid = fork();

    switch(pid){
        case -1:
            printf("creat process failed!\n");
            exit(1);
        case 0:
            printf("Child process is running!\n");
            printf("my pid = %d,my parent pid = %d\n",getpid(),getppid());
            printf("uid = %d,gid = %d\n",getuid(),getgid());
            execve("processimage",argv,environ);
            printf("neve go here!");
        default:
            printf("parent process is running!\n");
            break;
    }
    wait(&stat_val);
    //_wait(&stat_val)
    exit(0);
}
