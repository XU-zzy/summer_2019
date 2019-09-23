
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

int ticket=13;
//定义一把锁
pthread_mutex_t mutex;
void* route(  void *arg)
{
      char *id=(  char*)arg;
      for(int i = 0;i < 3;i++)
      {
            //加锁
            pthread_mutex_lock(&mutex);

                  sleep(1);
                  printf(  "%s sells ticket %d\n",id,ticket);
                  ticket--;
                  //释放锁
                  pthread_mutex_unlock(&mutex);

      }
}

int main(  )
{

      //初始化锁
      pthread_mutex_init(&mutex,NULL);
      pthread_t t1,t2,t3,t4;
      pthread_create(&t1,NULL,route,"thread_1");
      pthread_create(&t2,NULL,route,"thread_2");
      pthread_create(&t3,NULL,route,"thread_3");
      pthread_create(&t4,NULL,route,"thread_4");

      pthread_join(t1,NULL);
      pthread_join(t2,NULL);
      pthread_join(t3,NULL);
      pthread_join(t4,NULL);
      //销毁锁
      pthread_mutex_destroy(&mutex);
      return 0;

}

