#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
int globvar = 0;
//int var = 0;

int main(){
    pid_t pid;
    int i;
    int var = 0;
    printf("fork is different with fork\n");
    pid = fork();
    //pid = vfork();
    switch(pid){
        case 0:
            i = 3;
            while(i--){
                printf("Child is running\n");
                globvar++;
                var++;
                sleep(1);
            }
                //exit(0);
            //}
            printf("child's globvar = %d,var = %d\n",globvar,var);
            exit(0);//必须加这个，不然不正常退出子进程会导致父进程var乱码
            break;
        
        case -1:
            perror("something wrong!\n");
            exit(0);
        default:
            i = 5;
            while(i--){
                printf("Parent is running\n");
                globvar++;
                var++;
                sleep(1);
            }
            printf("Parent's globvar = %d,var = %d\n",globvar,var);
            exit(0);
    }
}

