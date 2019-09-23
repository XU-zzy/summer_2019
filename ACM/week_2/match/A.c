#include <stdio.h>
#include<algorithm>
bool cmp(int a,int b){
    return a > b;
}
int main()
{
    int n,m;
    scanf("%d %d",&n,&m);
    int a[1000+10];
    for(int i = 0;i < n;i++){
        scanf("%d",&a[i]);
    }
    
    std::sort(a,a+n,cmp);
    
    int i;
    for(i = 0;i < n;i++){
        if(m <= 0)
            break;
        m -= a[i];
    }
    
    printf("%d\n",i);
    return 0;
}

