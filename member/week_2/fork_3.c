#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
int main(){
    pid_t pid;
    
    pid = fork();
    switch(pid){
        case -1:
            printf("创建进程错误！\n");
            exit(-1);
        case 0:
            while(1){
                printf("子进程（孤儿进程）id:%d\t父进程id:%d\n",getpid(),getppid());
                sleep(1);
            }
        default:
            printf("父进程id:%d\n",getpid());
            exit(0);
    }
    return 0;
}
