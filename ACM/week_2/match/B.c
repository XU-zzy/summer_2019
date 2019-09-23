#include <stdio.h>
#include<string.h>
#include<algorithm>
int main()
{
    int n,a[4],t;
    scanf("%d",&n);
    for(int i = 1;i < 4;i++)
        a[i] = 0;
    
    for(int i = 1;i < n+1;i++){
        scanf("%d",&t);
        a[t] = a[t] + 1;
    }
    std::sort(a+1,a+4);
    printf("%d\n",n-a[3]);
    return 0;
}

