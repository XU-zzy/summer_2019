#include<stdio.h>
#include<algorithm>
#define maxn 4000
using namespace std;
int map1[maxn*maxn];
int map2[maxn*maxn];
int a[maxn],b[maxn],c[maxn],d[maxn];
int main(){
      int n,i,j,sum,p;
      scanf("%d",&n);
      for(i=0;i<n;i++)
         scanf("%d%d%d%d",&a[i],&b[i],&c[i],&d[i]);  
      //分别将两列分项相加，变成两个n*n的数组
      for(i=0;i<n;i++)
        for(j=0;j<n;j++)
         map1[i*n+j]=a[i]+b[j];
      for(i=0;i<n;i++)
        for(j=0;j<n;j++)
          map2[i*n+j]=c[i]+d[j];
      //排序，准备二分
      sort(map1,map1+n*n);
      sort(map2,map2+n*n);
      sum=0;
      p=n*n-1;
      
      for(i=0;i<n*n;i++){ 
        while(p>=0&&map1[i]+map2[p]>0) 
            p--;
        if(p<0) 
            break;
        int temp=p;
        while(temp>=0&&map1[i]+map2[temp]==0){
            sum++; 
            temp--;
        }
      }
      printf("%d\n",sum);
    return 0;
}
