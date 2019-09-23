#include <stdio.h>
#include<unistd.h>
#include<pthread.h>
pthread_mutex_t     mutex;  //互斥锁
pthread_cond_t      cond;   //条件变量

void *thread1(void *arg){
    /* pthread_cleanup_push(pthread_mutex_unclock,&mutex); */

    while(1){
        printf("thread1 is running!\n");
        
        pthread_mutex_lock(&mutex); //加锁
        pthread_cond_wait(&cond,&mutex);//该条件变量指向互斥锁

        printf("thread1 applied the condition!\n");
        pthread_mutex_unlock(&mutex);//开锁0

        sleep(3);
    }
}

void *thread2(void  *arg){
    while(1){
        printf("thread2 is  running!\n");
        
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex);
        
        printf("thread2 applied the condition!\n");
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

int main()
{
    pthread_t tid1,tid2;

    printf("condition variable study!\n");
    //初始化互斥锁和条件变量
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    pthread_create(&tid1,NULL,(void *)thread1,NULL);
    pthread_create(&tid2,NULL,(void *)thread2,NULL);

    do{
        pthread_cond_signal(&cond);
    }while(1);

    sleep(10);
    return 0;
}

