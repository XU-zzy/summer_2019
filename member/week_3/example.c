#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

//消费者进程
void *thread_consumer(void *ptr); 
//生产者进程
void *thread_producer(void *ptr);

#define MAX 100000   /*生产者需要生产的数量*/
pthread_mutex_t the_mutex;  /*互斥量*/
pthread_cond_t condc,condp;/*生产者和消费者的线程条件变量*/

//初始时缓冲区中没有数据
int buffer=0;
int buffer_max=0;/*用于记录缓冲区最多被用了多少*/

#define BUFFER_SIZE 10000    /*缓冲区大小*/

/*消费者线程*/
void *thread_consumer(void *arg)
{
    int i;
    //消费完指定数目 退出线程
    for(i=0;i<MAX;i++)
    {
        usleep(1000);
        //互斥量加锁，线程同步
        pthread_mutex_lock(&the_mutex);
        /*缓冲区空  该进程睡眠   睡眠时pthread_cond_wait函数会对互斥量the_mutex解锁，这样生产者线程可以正常工作*/
        while(buffer==0)
        {
            pthread_cond_wait(&condc,&the_mutex);
        }
        //缓冲区非空 此时pthread_cond_wait又会对互斥量再次加锁
        --buffer;  //消耗一个元素
        printf("consume an element, buffer=%d\n",buffer);  //打印缓冲区中剩余元素
        //如果缓冲区中元素个数为0   唤醒生产者线程
        if(buffer==0)
        {
            pthread_cond_signal(&condp);
        }
        //释放互斥量
        pthread_mutex_unlock(&the_mutex);
    }
    pthread_exit(NULL);
}

/*生产者线程*/
void *thread_producer(void *ptr)
{
    int i;
     //消费完指定数目 退出线程
    for(i=0;i<MAX;i++)
    {
         usleep(1000);
         //互斥量加锁，线程同步
         pthread_mutex_lock(&the_mutex);
          //缓冲区满， 该进程睡眠 睡眠时pthread_cond_wait函数会对互斥量the_mutex解锁，这样消费者线程可以正常工作*/
         while(buffer==BUFFER_SIZE)
         {
             pthread_cond_wait(&condp,&the_mutex);
         }
         // 缓冲区未满时 此时pthread_cond_wait又会对互斥量再次加锁
         ++buffer;//生产者增加一个数据
         /*记录缓冲区最大的数据个数*/
         if(buffer>buffer_max)
                buffer_max=buffer;
         printf("produce an element, buffer=%d\n",buffer);
         //如果缓冲区中元素个数大于0   唤醒消费者线程
         if(buffer>0)
         {
             pthread_cond_signal(&condc);
         }
         //释放互斥量
         pthread_mutex_unlock(&the_mutex);
    }
}

int main()
{
    pthread_t pro,con;
    //初始化互斥量
    pthread_mutex_init(&the_mutex,0);
    //初始化线程条件变量
    pthread_cond_init(&condc,0);
    pthread_cond_init(&condp,0);
    //创建生产者、消费者两个线程
    pthread_create(&con,NULL,thread_consumer,NULL);
    pthread_create(&pro,NULL,thread_producer,NULL);
    //等待两个线程结束
    pthread_join(pro,0);
    pthread_join(con,0);
    //清除变量
    pthread_cond_destroy(&condc);
    pthread_cond_destroy(&condp);
    pthread_mutex_destroy(&the_mutex);
    printf("buffer_max=%d",buffer_max);
}
