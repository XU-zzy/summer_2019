#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/* #include<unistd.h> */
int main(){
    int N;
    scanf("%d",&N);
    int left[10000],right[10000];
    int i;
    int l,m;
    while(N--){
        scanf("%d %d",&l,&m);
        l *= 100;
        if(m == 0){
            printf("0\n");
            continue;
        }

        char bank[10];
        int left_n = 0,right_n = 0,n;
        /* printf("\n1\n"); */
        for(i = 0;i < m;i++){   //输入并分别存储数据
            scanf("%d %s",&n,bank);
            if(bank[0] == 'l')
                left[left_n++] = n;
            else
                right[right_n++] = n;
        }
        /* printf("\n2\n"); */
        int len_car;
        int times = 0;
        for(int i = 0,j = 0;i < left_n || j < right_n;){    //开始模拟
            len_car = 0;    //装货长度
            times++;        //去右边
            
            if(i == left_n && j == right_n){ // 货物已经全部运送完毕，不必去右侧
                times--;
                break;
            }

            while(i < left_n && len_car + left[i] <= l){    //左侧装货
                len_car += left[i];
                i++;
            }
            /* printf("\ni = %d,j = %d,left_n = %d,right_n = %d\n",i,j,left_n,right_n); */
            //sleep(1);
            len_car = 0;    //长度清零
            times++; //回到左侧

            if(i == left_n && j == right_n) //全到了右边，不必回到左侧
                times--;

            while(j < right_n && len_car + right[j] <= l){//右侧装货
                len_car += right[j];
                j++;
            }
            /* printf("\n4\n"); */
        }
        printf("%d\n",times);
    }
    return 0;
}
