#include<stdio.h>
#include<algorithm>
int main(){
    int a[4];
    for(int i = 0;i < 4;i++){
        scanf("%d",&a[i]);
    }
    
    std::sort(a,a+4);
    
    for(int i = 0;i < 2;i++)
        if(a[i] + a[i+1] > a[i+2]){
            printf("TRIANGLE\n");
            return 0;
        }
    for(int i = 0;i < 2;i++)
        if(a[i] + a[i+1] == a[i+2]){
            printf("SEGMENT\n");
            return 0;
        }
    
    printf("IMPOSSIBLE\n");
    return 0;
}
