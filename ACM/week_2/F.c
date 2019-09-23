#include<stdio.h>
int main(){
    int n;
    scanf("%d",&n);
    int num_2,num_1;
    num_1 = n%2;
    num_2 = n/2;
    if(n == 0){
        printf("1");
        printf("\n");
        return 0;
    }
    if(n<=36){
        for(int i = 0;i < num_2;i++){
            printf("8");
        }
        for(int i = 0;i < num_1;i++){
            printf("9");
        }
    }
    else{
        printf("-1");
    }
    printf("\n");
    return 0;
}
