**实验过程**

先猜想一下这个程序的运行结果。假如运行“```./process 20```”，输出会是什么样？
然后按照注释里的要求把代码补充完整，运行程序。
开另一个终端窗口，运行“ps aux|grep process”命令，看看process 究竟启动了多少个进程。
回到程序执行窗口，按“数字键+回车”尝试杀掉一两个进程，再到另一个窗口看进程状况。
按q 退出程序再看进程情况。

- 回答下列问题

1. 你最初认为运行结果会怎么样？
答：最初认为它只会生成10个进程,之后输入一个少一个进程，最后按q退出后，进程全部结束
2. 实际的结果什么样？有什么特点？试对产生该现象的原因进行分析。
答：符合，总共10个进程，从0~9持续输出，然后删一个进程少一个进程，直接用q的话，所有进程都会停止
3. proc_number 这个全局变量在各个子进程里的值相同吗？为什么？
答：不同，
    因为 proc_number 是在进入每个进程后，重新赋值，子进程间资源相互独立，所以互不影响
4. kill 命令在程序中使用了几次？每次的作用是什么？执行后的现象是什么？
答：一共使用了三次，
    每次的作用,执行后的现象，都在源程序中注释出来（见注释：------by zzy）
5. 使用kill 命令可以在进程的外部杀死进程。进程怎样能主动退出？这两种退出方式哪种更好一些？
答：调用exit()正常退出
    但是当kill命令使子进程在父进程之前退出，但是父进程又没有wait，那么将会形成僵死进程
    所以调用exit()正常退出还是好一点，比较安全,避免僵尸进程
