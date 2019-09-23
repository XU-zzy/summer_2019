#include<stdio.h>
int main(){
    int n;
    scanf("%d",&n);

    int i = 0,k = 1;
    //常规方法
    /*while(n > 0){
        n -= k;
        k *= 2;
        i++;
    }
    */

    //二进制方法
    //效率更高
    while(n){
        /* n -= k; */
        /* k = k << 1; */
        /* printf("n = %d\n",n); */
        n = n >> 1;
        i++; 
    }

    printf("%d\n",i);
}



