#include<stdio.h>
#include<algorithm>
int main(){
    int m,n,a[2010];
    scanf("%d %d",&n,&m);
    int i;
    for( i = 0;i < n;i++){
        scanf("%d",&a[i]);
    }
    std:: sort(a,a+i);
    int ans = 0;
    for(int i = 0;i < m;i++){
        if(a[i]<=0)
            ans += a[i];
        else
            break;
        //printf("%d ",a[i]);
    }
    printf("%d\n",-ans);
}
