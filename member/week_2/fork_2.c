#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
int main(){
    int pid;
    char * msg;
    int k;
    printf("Process Creation Study\n");
    pid = fork();
    switch(pid){
    case 0:
        msg = "Child running";
        k = 3;
        break;
    case -1:
        perror("Wrong!\n");
        break;
    default:
        msg = "Parent is running";
        k = 5;
        break;
    }
    while(k>0){
        puts(msg);
        sleep(1);
        k--;
    }
    exit(0);
}
