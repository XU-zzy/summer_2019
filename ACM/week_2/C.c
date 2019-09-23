#include<stdio.h>
int main(){
    //int t;
    int n,d,flag;
    scanf("%d %d",&n,&d);
    int max = 0,count = 0;
    char num[120];
    for(int i = 0;i < d;i++){
        flag = 1;
        scanf("%s",num);
        for(int j = 0;j < n;j++){
            //printf("t = %d\n",t);
            if(num[j] == '0'){
                flag = 0;
                break;
            }
        }
        if(flag == 0){
            count++;
        }
        if(count > max){
            max = count;
        }
        if(flag == 1){
            count = 0;
        }
        //printf("max = %d,count = %d\n",max,count);
    }
    printf("%d\n",max);
    return 0;
}
