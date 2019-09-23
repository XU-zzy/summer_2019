#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
pthread_mutex_t mutex[2];
pthread_cond_t cond[2];
int a[100];
void *add(void *arg){
    for(int i = 0;i < 100;i++){
        usleep(100000);
        /* pthread_mutex_lock(&mutex[0]); */
        while(a[i] == 0){
            pthread_mutex_lock(&mutex[0]);
            a[i] = 1;
            printf("%d goods has been add!\n",i);
        }
        if(i == 99)
            i = 0;
        usleep(1);
        pthread_cond_signal(&cond[0]);
        pthread_mutex_unlock(&mutex[0]);
    }
}

void *del(void *arg){
    for(int i = 0;i < 100;i++){
        usleep(100000);
        pthread_mutex_lock(&mutex[0]);
        pthread_cond_wait(&cond[0],&mutex[0]);
        while(a[i] == 1){
            a[i] = 0;
            printf("%d goods has been sold!\n",i);
        }
        if(i == 99)
            i = 0;
        /* pthread_cond_signal(&cond[0]); */
        pthread_mutex_unlock(&mutex[0]);

    }
}

int main(){
   
    pthread_t jia,jian;
    memset(a,0,100);
    for(int i = 0;i < 2;i++){
        pthread_mutex_init(&mutex[i],NULL);
        pthread_cond_init(&cond[i],NULL);
    }
    
    pthread_create(&jia,NULL,add,NULL);
    pthread_create(&jian,NULL,del,NULL);
    
    /* sleep(1); */
    /* pthread_cond_signal(&cond[0]); */
    /* pthread_cond_signal(&cond[0]); */
    
    pthread_join(jia,NULL);
    pthread_join(jian,NULL);
}
