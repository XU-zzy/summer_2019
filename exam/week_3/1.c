#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>
#include <pthread.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct task{
   void *(*process) (void*arg);//函数指针，指向任务
    void *arg;                  //回调函数的参数
    struct task *next;          //指向下一个任务
}task_t;
typedef struct thread_pool{
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_cond;
    task_t* queue_head;
    int shutdown;
    pthread_t *threadid;
    int max_thread_num;
    int cur_queue_size;
}thread_pool;
void threadpool_init(int max_thread_num);
int threadpool_destroy();
void *thread_routine();
int pool_add_task(void*(*process)(void*arg),void *arg);


static thread_pool* pool = NULL;
//线程池中加入任务
int pool_add_task(void*(*process)(void*arg),void *arg)
{
    int ret = 0;
    //构造一个新的任务小节点
    task_t *new_task = (task_t *)malloc(sizeof(task_t));
    new_task->process = process;
    new_task->arg = arg;
    new_task->next = NULL;//初始化为空
    pthread_mutex_lock(&(pool->queue_lock));

    //将任务加入到等待队列中
    task_t * cur_task = pool->queue_head;
    if(cur_task != NULL)
    {
        while(cur_task->next != NULL)
            cur_task = cur_task->next;
        cur_task->next = new_task;
    }
    else
    {
        pool->queue_head = new_task;
    }
    //连接新任务
    
    if(pool->queue_head == NULL)
    {
        ret = -1;
        printf("func pool_add_worker err:%d",ret);
        return ret;
    }

    pool->cur_queue_size++;
    pthread_mutex_unlock(&(pool->queue_lock));

    pthread_cond_signal(&(pool->queue_cond));
    return ret;
}
//任务接口函数，子线程统一调用这个函数，这个函数检查任务队列中的任务
void *thread_routine(void *arg)
{
    /* printf("线程[0x%lx]加入线程池\n",pthread_self()); */

    //死循环保持所有线程
    while(1)
    {
        pthread_mutex_lock (&(pool->queue_lock));//给线程加锁
        
        //无任务状态和不销毁时，线程阻塞等待
        while(pool->cur_queue_size == 0 && pool->shutdown != 1)
        {
            /* printf("线程[0x%lx]正在等待\n",pthread_self()); */
            pthread_cond_wait(&(pool->queue_cond),&(pool->queue_lock));//基于条件变量阻塞 
        }
        
        if(pool->shutdown)//线程池要销毁
        {
            //先解锁后结束
            pthread_mutex_unlock(&(pool->queue_lock));
            /* printf("线程[0x%lx]将要销毁\n",pthread_self()); */
            pthread_exit(NULL);
        }

        /* printf("线程[0x%lx]将要执行任务\n",pthread_self()); */

        //断言函数
        assert(pool->cur_queue_size != 0);
        assert(pool->queue_head != NULL);

        //任务队列长度-1,取出队首节点就是要执行的任务
        pool->cur_queue_size--;
        task_t * cur_task = pool->queue_head;
        pool->queue_head = cur_task->next;
        //解锁
        pthread_mutex_unlock(&(pool->queue_lock));
        (*(cur_task->process))(cur_task->arg);//函数指针
        free(cur_task);
        cur_task = NULL;
    }
    printf("线程[0x%lx]异常退出线程池\n",pthread_self());
}

//线程池初始化
//pool表示指向头结点的一个指针，max_thread_num表示线程池中最大的线程数
void threadpool_init(int max_thread_num)
{
    //对头指针的初始化
    pool = (thread_pool*)malloc(sizeof(thread_pool));

    pthread_mutex_init(&(pool->queue_lock),NULL);
    pthread_cond_init(&(pool->queue_cond),NULL);
    pool->queue_head = NULL;
    pool->max_thread_num = max_thread_num;
    pool->cur_queue_size = 0;
    pool->shutdown = 0;
    pool->threadid = (pthread_t *)malloc(max_thread_num*sizeof(pthread_t));
    //thread_t 表示线程的标示
    
    for(int i = 0;i<max_thread_num;i++)
    {
        pthread_create(&(pool->threadid[i]),NULL,thread_routine,NULL);
    }
    
}

//销毁线程池中的所有线程，清空任务队列
//pool指向头节点的指针
int threadpool_destroy()
{
    int ret = 0;
    if(pool->shutdown)
    {
        ret = -1;
        printf("多次销毁线程池:%d\n",ret);
        return ret;//防止多次调用
    }
    pool->shutdown = 1;
    
    //唤醒所有线程,以便于销毁
    pthread_cond_broadcast(&(pool->queue_cond));

    for(int i = 0;i<pool->max_thread_num;i++)
    {
        //阻塞主线程，结束这些线程
        pthread_join((pool->threadid[i]),NULL);
    }
    free(pool->threadid);

    //销毁任务队列
    task_t *cur = NULL;//辅助指针遍历整个链表
    while(pool->queue_head != NULL)
    {
        cur = pool->queue_head;
        pool->queue_head = pool->queue_head->next;
        free(cur);
    }

    //销毁互斥锁和条件变量
    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_cond));
    free(pool);
    pool  = NULL;
    return ret;

}

