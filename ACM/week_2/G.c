#include <stdio.h>
#include<algorithm>
typedef struct node{
    double mao;
    double shu;
}s_input;
bool cmp(s_input a,s_input b){
    return a.mao/a.shu > b.mao/b.shu;
}
int main()
{
    int n;
    double sum;
    while(1){
        scanf("%lf %d",&sum,&n);
        if(sum == -1 && n == -1)
            break;

        s_input message [1010];
        for(int i = 0;i < n;i++){
            scanf("%lf %lf",&message[i].mao,&message[i].shu);
        }
        
        std::sort(message,message+n,cmp);
        
        int j = 0;
        double ans = 0,flag;
        while(n--){     //一定要是 n-- 不能是sum > 0，因为不一定会把 sum 用完
            flag = sum;
            sum -= message[j].shu;
            /* printf("sum = %lf\n",sum); */
            if(sum <= 0){
                ans = ans + ((flag*message[j].mao)/message[j].shu);
                /* printf("ans_l = %lf\n",ans); */
                break;
            }

            ans += message[j].mao;
            /* printf("ans = %lf\n",ans); */
            j++;
        }
        printf("%.3lf\n",ans);
    }
    return 0;
}

