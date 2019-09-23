#include <stdio.h>
#include<algorithm>
typedef struct node{
    int t_start;
    int t_end;
}node;
bool cmp(node a,node b){
        return a.t_end < b.t_end;
}
int main()
{
    while(1){
        int n;
        scanf("%d",&n);
        if(n == 0)
            break;

        node times[110];
        for(int i = 0;i < n;i++)
            scanf("%d %d",&times[i].t_start,&times[i].t_end);

        std::sort(times,times+n,cmp);

        int count = 0;
        int end = 0;
        for(int i = 0;i < n;i++){
            if(times[i].t_start >= end){ 
                end = times[i].t_end;
                count++;
            }
        }
        printf("%d\n",count);
    } 
    return 0;
}
