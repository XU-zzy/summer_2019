#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>

int main(){
    pid_t pid;
    char *str;
    int k;
    int exit_code;

    printf("Study how to get exit code\n");
    pid = fork();
    switch(pid){
        case -1:
            printf("Process creation failed!\n");
            break;
        case 0:
            str = "Child process is running!";
            k = 3;
            exit_code = 6;
        default:
            exit_code = 0;
            break;
    }

    if(pid != 0){
        int stat_val;
        pid_t child_pid;
        
        child_pid = wait(&stat_val); //返回子进程id

        printf("Child process has exited,pid = %d\n",child_pid);

        if(WIFEXITED(stat_val)){
            printf("Child process has exit with code %d\n",WEXITSTATUS(stat_val));
        }
        else{
            printf("Child exit abnormally\n");
        }
    }else{
        while(k--){
            puts(str);
            sleep(1);
        }
    }
    exit(exit_code);
}
