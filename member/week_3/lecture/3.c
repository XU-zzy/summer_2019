#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/syscall.h>
pid_t gettid()
{
     return syscall(SYS_gettid);
}
void *f(void *arg) {

    if(*(int *)arg){
        printf("tid 父进程所属线程 ID 为 %d\n",gettid());
        printf("pid 父进程所属线程 ID 为 %ld\n",pthread_self());
    }
    else{
        printf("tid 子进程所属线程 ID 为 %d\n",gettid());
        printf("pid 子进程所属线程 ID 为 %ld\n",pthread_self());
    }
    pthread_exit(NULL);

}
int main(){
    pid_t pid1;
    pthread_t tid1;
    pthread_t tid2;
    int *retval_1;
    int *retval_2;
    int a;
    pid1 = fork();
    
    if(pid1 == 0){
        a = 0;
        printf("pid 子进程主线程 ID 为 %ld\n",pthread_self());
        printf("tid 子进程主线程 ID 为 %d\n",gettid());
        pthread_create(&tid1,NULL,f,(void *)&a);
    }
    else{
        a = 1;
        printf("pid 父进程主线程 ID 为 %ld\n",pthread_self());
        printf("tid 父进程主线程 ID 为 %d\n",gettid());
        pthread_create(&tid2,NULL,f,(void *)&a);
    }
    pthread_join(tid1,(void**)&retval_1);
    pthread_join(tid2,(void**)&retval_2);
}
