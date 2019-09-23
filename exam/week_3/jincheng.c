#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <stdlib.h>
/* 允许建立的子进程个数最大值 */
#define MAX_CHILD_NUMBER 10
/* 子进程睡眠时间 */
#define SLEEP_INTERVAL 2
int proc_number=0;
/* 子进程的自编号，从0开始 */
void do_something();

int main(int argc, char* argv[])
{
/* 子进程个数 */
	int child_proc_number = MAX_CHILD_NUMBER;
	int i, ch;
	pid_t  child_pid;
	pid_t pid[10]={0}; /* 存放每个子进程的id */
	if (argc > 1) /* 命令行参数第一个参数表示子进程个数*/
    {
        child_proc_number = atoi(argv[1]);
        child_proc_number= (child_proc_number > 10) ? 10 :child_proc_number;

    }

    pid_t pid_t;
    for (i=0; i<child_proc_number; i++) {
    /* 填写代码，建立child_proc_number个子进程要执行
    * 父进程把子进程的id保存到pid[i] */
        if((pid_t = fork()) < 0){
            printf("creat fork error!\n");
            exit(1);
        }
        if(pid_t == 0){
            proc_number = i;
            /* printf("--proc_number(%d) = %d",i,proc_number); */
            do_something();
        }
        if(pid_t != 0 && pid_t!= -1){
            pid[i] = pid_t;            
        }
    }
/* 让用户选择杀死进程，数字表示杀死该进程，q退出 */
	while ((ch = getchar()) != 'q')
    {
        if (isdigit(ch))
        {
        /*  填写代码，向pid[ch-'0']发信号SIGTERM，
        * 杀死该子进程 */
            kill(pid[ch - '0'],SIGTERM);    //杀死该子进程 ------by zzy
        /* pid[ch-'0'] = -1; */
        }
    }
/* 在这里写代码，杀死本组的所有进程 */
    for(int i = 0;i <= proc_number;i++){//杀死每个进程
        kill(pid[i],SIGTERM);   //逐个杀死进程 ------by zzy
    }    
    kill(0,SIGTERM);   //杀死主进程 ------by zzy
	return 0;
}
void do_something()
{
    for(;;)
    {
        printf("This is process No.%d and its pid is %d\n",proc_number,  getpid());
        sleep(SLEEP_INTERVAL); // 主动阻塞两秒钟
    }
}

