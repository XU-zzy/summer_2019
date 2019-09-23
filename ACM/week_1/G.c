#include<stdio.h>
#include<math.h>
#include<unistd.h>
int main(){
    long long int  sum,k = 1,v = 0,mid,low=1;
    scanf("%lld %lld",&sum,&k);
    long long int high = sum,ans = 0,min = 99999999999999;
    int flag = 1;
    while(low <= high && flag){
        //printf("%lld",v);
        if(low == high)
            flag = 0;
        mid = (low + high)/2;
        ans = 0;
        for(int i = 0;(int)(mid/pow(k,i)) > 0;i++){
            ans += (mid/pow(k,i));
            //printf("ans = %lld mid = %lld   %d\n",ans,mid,(int)(mid/pow(k,i)));
            //sleep(1);
        }
    
        if(ans >= sum){
            high = mid - 1;
            //保存答案，重点！！！！！！！！
            min = mid;
            //---------------
        }
        if(ans < sum){
            low = mid + 1;
        }
        //printf("high = %lld low = %lld min = %lld\n",high,low,min);
        //sleep(1);
    }
    printf("%lld",min);
    return 0;
}
