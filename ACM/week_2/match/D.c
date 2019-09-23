#include <stdio.h>
#include<algorithm>
bool cmp(int a,int b){
    return a > b;
}
int main()
{
    int n,a[13];
    scanf("%d",&n);
    for(int i = 1;i < 13;i++)
        scanf("%d",&a[i]);
    std::sort(a+1,a+13,cmp);
    int sum = 0;
    int i;
    for( i = 1;i < 13;i++){
        if(sum >= n)
            break;
        sum += a[i];
    }
    if(n == 0){
        printf("0\n");
        return 0;
    } 
    if(sum < n)
        printf("-1\n");
    else {
        printf("%d\n",i-1);
    }
    return 0;
}

