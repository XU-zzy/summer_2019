#include<stdio.h>
#include<algorithm>
using namespace std;
int main(){
    int n;
    scanf("%d",&n);
    
    int a[200010];
    for(int i = 1;i < n+1;i++){
        scanf("%d",&a[i]);
    }
    sort(a+1,a+n+1);
    /*for(int i = 1;i < n;i++){
        for(int j = i+1;j < n+1;j++){
            if(a[i] > a[j]){int t = a[i];a[i] = a[j];a[j] = t;}
        }
    }*/
    int count = 0;
    for(int i = 1,j = 1;i < n+1;i++){    
        if(a[i] >= j){
            //printf("a[%d] = %d >= %d\n",i,a[i],j);
            count++;
            j++;
        }
    }
    printf("%d",count);
    return 0;
}
