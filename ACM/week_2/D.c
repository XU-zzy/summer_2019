#include<stdio.h>
 
int arr[1100];
int main()
{
    int n,a,b,c,t,ans=0,sum=0;
    scanf("%d %d %d %d %d",&n,&a,&b,&c,&t);
    for(int i=0;i<n;i++)
    {
        scanf("%d",&arr[i]);
        sum+=t-arr[i];
    }
    ans=n*a;
    if(b>=c)
    {
         printf("%d\n",ans); 
    }
    else
    {
        printf("%d\n",ans+sum*(c-b));
    }
    return 0;
}
