#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
pthread_mutex_t mutex;
pthread_cond_t cond;

//线程1的执行任务
void * run1(void *arg){
    while(1){
        //等待被唤醒后执行任务
        pthread_cond_wait(&cond,&mutex);
        printf("hello!\n");
    }
}

//线程2执行的任务
void * run2(void *arg){
    while(1){
        //每隔1s唤醒线程1，提醒可以执行任务了
        pthread_cond_signal(&cond);
        sleep(1);
    }
}

int main(){
    //初始化互斥锁
    pthread_mutex_init(&mutex,NULL);
    //初始化条件变量
    pthread_cond_init(&cond,NULL);
    pthread_t t1,t2;
    //创建线程1,2
    pthread_create(&t1,NULL,run1,"thread_1");
    pthread_create(&t2,NULL,run2,"thread_2");
    //等待线程
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    //销毁互斥锁和条件变量
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}
