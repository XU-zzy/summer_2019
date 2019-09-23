#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
int main(){
    pid_t pid;

    printf("Process Creation Study\n");
    pid = fork();
    switch(pid){
    case 0:		//此时为子进程
        printf("Chid running,CurPid is %d,ParentPid is %d\n",pid,getppid());
        break;
    case -1:	//出错
        perror("Wrong!\n");
        break;
    default:	//此时父进程
        printf("Parent is running,ChildPid is %d,ParenrPid is %d\n",pid,getpid());
        break;
    }
    exit(0);
}
