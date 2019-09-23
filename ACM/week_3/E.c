#include<stdio.h>
#include<string.h>

#define MAX 100000+10

struct node {
    int weizhi;
    int buhsu;
}a[MAX],next;

int book[MAX];  //座位标记

int bfs(int n,int k){
    int head = 1,tail = 2;
        //初始化队列数据
        a[head].weizhi = n;
        a[head].buhsu = 0;
        
        //用于记录该点是否走过
        book[n] = 1;
    while(head < tail){
        //三种方式的走法逐一进行尝试
        for(int i = 0;i < 3;i++){
            if(i == 0){
                next.weizhi = a[head].weizhi + 1;
            }
            else if(i == 2){
                next.weizhi = a[head].weizhi - 1;
            }
            else{
                next.weizhi = a[head].weizhi * 2;
            }
            
            //步数赋值
            next.buhsu = a[head].buhsu + 1;
            
            //越界
            if(next.weizhi < 0 || next.weizhi > 100000)
                continue;
            
            //没有走过该点，该点如队
            if(book[next.weizhi] == 0){
                book[next.weizhi] = 1;
                a[tail] = next;     //注意，这里这寄给结构体进行了赋值
                tail++;
            }
            
            //到达终点，返回步数
            if(next.weizhi == k){
                return next.buhsu;
            }
        }
        head++;     //三种情况都试过以后，进入下一层
    }
    return 0;
}

int main(){
    int n,k;

    //初始化标记
    memset(book,0,MAX);
    
    scanf("%d %d",&n,&k);
    if(n > k){  //牛在人的左边，只能通过一种方式走（左一）
        printf("%d\n",n-k);
    }
    else if(n == k){    //本来就在一起
        printf("0\n");
    }
    else{   //进行搜索
        printf("%d",bfs(n,k));
    }

    return 0;
}
