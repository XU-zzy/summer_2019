//线程终止
//使用 pthread_join 等待，辅助线程结束
//
//输出结果，该函数阻塞了主线程，让其等待 assisrhread 结束
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
void assisthread(void *arg){
    printf("I am helping to do some jobs!\n");
    sleep(1);
    pthread_exit(0);
}
int main(){
    pthread_t assisth_id;
    int status;

    pthread_create(&assisth_id,NULL,(void *)assisthread,NULL);
    pthread_join(assisth_id,(void *)&status);
    printf("assistthread's eixt is caused %d\n",status);

    return 0;
}
