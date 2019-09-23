#include<stdio.h>
int main(){
    int N;
    scanf("%d",&N);
    int sum,m;
    while(N--){
        scanf("%d %d",&sum,&m);
        if(sum%(m+1) == 0){
            printf("second\n");
        }
        else{
            printf("first\n");
        }
    }
    return 0;
}
