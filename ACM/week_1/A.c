#include<stdio.h>
#include<string.h>
int main(){
    long long int n;
    scanf("%lld",&n);
    
    long long  num;
    while(n--){
        scanf("%lld",&num);
        long long  a[6],count = 0,flag = 0;
        memset(a,0,sizeof(a));
        while(num != 1){
            flag = num;
            //printf("\n%lld\n",num);
            if(num % 2 == 0){
                num/=2;
                count++;
            }
            if(num % 3 == 0){
                num = (num/3)*2;
                count++;
            }
            if(num % 5 == 0){
                num = (num/5)*4;
                count++;
            }
            //printf("\n%lld\n",num);
            if(flag == num && flag != 1){
                count = -1;
                break;
            }
            //printf("%lld",num);
        }
        printf("%lld\n",count);
    }
    return 0;
}
