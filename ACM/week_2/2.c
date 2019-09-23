#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
pthread_mutex_t chopstick[6] ;
//获得左右筷子的编号
void get(int *left,int *right,char phi) {
    switch (phi){
        case 'A':
            *left = 5;
            *right = 1;
            break;
        case 'B':
            *left = 1;
            *right = 2;
            break;
        case 'C':
            *left = 2;
            *right = 3;
            break;
        case 'D':
            *left = 3;
            *right = 4;
            break;
        case 'E':
            *left = 4;
            *right = 5;
            break;
    }
}

//就是一个抢夺资源释放资源的过程
void *eat_think(void *arg)
{
    //获取参数，判断是哪个哲学家在用餐
    char phi = *(char *)arg;
    int left,right;
    //获得左右筷子的编号
    get(&left,&right,phi);
    for(;;){
        //等待10微妙
        sleep(1);
        //先对左边的线程进行上锁，表示已经拿起了左边的筷子
        pthread_mutex_lock(&chopstick[left]);
        printf("Philosopher %c fetches chopstick %d\n", phi,left);
        //尝试对右边上锁
        if (pthread_mutex_trylock(&chopstick[right]) == EBUSY){
            //上锁失败说明右边已上锁，筷子正在被用，解锁左边筷子，返回重新来
            pthread_mutex_unlock(&chopstick[left]);
            continue;
        }
        //表示拿起了右边的筷子
        printf("Philosopher %c fetches chopstick %d\n", phi,right);
        //集齐两双筷子，该哲学家可以就餐
        printf("Philosopher %c is eating.\n",phi);
        sleep(1);
        //解锁，表示放下筷子
        pthread_mutex_unlock(&chopstick[left]);
        printf("Philosopher %c release chopstick %d\n", phi,left);
        pthread_mutex_unlock(&chopstick[right]);
        printf("Philosopher %c release chopstick %d\n", phi,right);
    }
}

int main(){
    pthread_t A,B,C,D,E; //5个哲学家
    int i;
    //五个哲学家分别初始化一个锁
    for (i = 0; i < 5; i++)
        pthread_mutex_init(&chopstick[i],NULL);
        
        //分别创建一个线程,代表不同哲学家,相邻两线程之间因为资源问题无法同时进行
        pthread_create(&A,NULL, eat_think, (void *)"A");
        pthread_create(&B,NULL, eat_think, (void *)"B");
        pthread_create(&C,NULL, eat_think, (void *)"C");
        pthread_create(&D,NULL, eat_think, (void *)"D");
        pthread_create(&E,NULL, eat_think, (void *)"E");
        
        pthread_join(A,NULL);
        pthread_join(B,NULL);
        pthread_join(C,NULL);
        pthread_join(D,NULL);
        pthread_join(E,NULL);
        
        return 0;
}
