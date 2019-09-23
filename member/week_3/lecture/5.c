#include<stdio.h>
#include<pthread.h>
pthread_mutex_t mutex;
pthread_cond_t cond;

void *f(void *arg) {
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main(){

    pthread_t tid1;
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);

    pthread_mutex_lock(&mutex);
    pthread_create(&tid1,NULL,f,(void *)&cond);

    pthread_cond_wait(&cond,&mutex);
    pthread_mutex_unlock(&mutex);
    printf("end!\n");
 
    return 0;

}
