#include<stdio.h>
#include<stdlib.h>
#include<algorithm>
using namespace std;
const int MAXN = 1010;
struct node
{
    double j,f;
    double r;
}a[MAXN];
/*
int cmp(const void *a,const void *b)//从大到小排序 
{
    struct node *c=(node *)a;
    struct node *d=(node *)b;
    if(c->r > d->r) return -1;
    else return 1;
}    */
bool cmp(node a,node b)
{
    return a.r  >  b.r;
}    
int main()
{
    int N;
    double M;
    double ans;
    while(scanf("%lf%d",&M,&N))
    {
        if(M==-1&&N==-1) break;
        for(int i=0;i<N;i++)
        {
           scanf("%lf%lf",&a[i].j,&a[i].f);
           a[i].r=(double)a[i].j/a[i].f;
        }    
        //qsort(a,N,sizeof(a[0]),cmp);
        sort(a,a+N,cmp);
        ans=0;
        for(int i=0;i<N;i++)
        {
            if(M>=a[i].f)
            {
                ans+=a[i].j;
                M-=a[i].f;
            }    
            else 
            {
                ans+=(a[i].j/a[i].f)*M;
                break;
            }    
        }   
        printf("%.3lf\n",ans); 
    }    
    return 0;
}
