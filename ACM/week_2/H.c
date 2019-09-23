#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<algorithm>
typedef struct node{
    int day;
    int loss;
}score;
//降序排序
bool cmp(score a,score b){
    return a.loss > b.loss;
}
int main()
{
    int N,n,ans;
    score mark[1010];
    scanf("%d",&N);
    while(N--){
        ans = 0;
        scanf("%d",&n);
        for(int i = 0;i < n;i++)
            scanf("%d",&mark[i].day);
        for(int i = 0;i < n;i++)
            scanf("%d",&mark[i].loss);
        //分值由大到小排序
        std::sort(mark,mark+n,cmp);

        int plan[1000];
        //初始化
        memset(plan,0,sizeof(plan));
        
        for(int i = 0;i < n;i++){
            if(plan[mark[i].day] == 0){
                plan[mark[i].day]++;
                /* printf("\nloss = %d\n",mark[i].loss); */
                continue;
            }
            /* printf("\ni = %d   %d\n",i,mark[i].loss); */
            int j = mark[i].day;
            while(j--){
                if(plan[j] == 1){
                    /* printf("plan[%d] = %d\n",j,plan[j]); */
                    continue;
                }
                else{
                    plan[j]++;
                    /* printf("plan[%d] = %d\n",j,plan[j]); */
                    break;
                }
            }
            if(j < 1){
                ans += mark[i].loss;
                /* printf("ans = %d\n",ans); */
            }
        }
        printf("%d\n",ans);
    }
    return 0;
}

