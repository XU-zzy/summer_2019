//可以看出，两线程之中的 tsd 互不干扰

#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
pthread_key_t key;  //定义全局变量key

void *thread2(void *arg){
    int tsd = 5;
    printf("thread %lu is running!\n",pthread_self());
    pthread_setspecific(key,(void *)tsd);    //将 tsd 和 key 相关联
    printf("thread %lu returns %d \n",pthread_self(),pthread_getspecific(key));
}

void *thread1(void *arg){
    int tsd = 1;
    pthread_t thid2;

    printf("thread %lu is running!\n",pthread_self());  //获取线程 id
    pthread_setspecific(key,(void *)tsd);   //将 tsd 和 key 相关联
    pthread_create(&thid2,NULL,thread2,NULL);
    sleep(2);
    printf("thread %lu returns %d\n",pthread_self(),pthread_getspecific(key));
}

int main(){
    pthread_t thid1;
    printf("main thread begins running\n");
    pthread_key_create(&key,NULL);  //在TSD池里分配一个 key 
    pthread_create(&thid1,NULL,thread1,NULL);  //建立一个线程
    sleep(3);
    pthread_key_delete(key);    //删除 key 
    printf("main thread exit!\n");
    return 0;
}

