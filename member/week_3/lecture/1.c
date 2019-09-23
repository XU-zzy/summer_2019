/*examp_1.c*/
#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
/* int a; */
void *f(void *arg) {
    printf("thread1 running\n");
    /*a = 111;
    pthread_exit((void *)&a);*/
    
    int a = 111;
    pthread_exit((void *)&a);

    /*int a = 111;
    int *p = (int*)malloc(sizeof(int));
    *p = a;
    pthread_exit((void*)p);*/
}

int main(int argc,char *argv[])

{
    pthread_t pid1;
    pthread_create(&pid1,NULL,f,NULL);
    int t;
    int *p = &t;
    pthread_join(pid1,(void **)&p);
    printf(" 线程结束的返回值为 :%d\n",*p);
    
    return 0;

}
