#include<stdio.h>
#include<string.h>
#include<math.h>
int hang[11];
int count;
int n;
void dfs(int lie){
    if (lie == n){//循环到第 n 个，数目+1
        count++;
        return;
    }
    int i,k;
    for(i = 0;i < n;i++){//判断第 lie 列，第 i 个
        for(k = 0;k < lie;k++){//在已经记录的 hang 中，第 k 列 第 hang[k] 行
            if(hang[k] == i || (hang[k] - i) == (k - lie) || (hang[k] - i) == (lie - k))
                break;
        }
        //没有重复的，录入并尝试下一个
        if(k == lie){
            hang[lie] = i;
            dfs(lie + 1);
        }  
    }
}

int main(){
    int a[11];
    for(int i = 0;i < 11;i++){
        count = 0;
        n = i;
        dfs(0);
        a[i] = count;
    }
    while(1){  
        scanf("%d",&n);
        if(n == 0)
            break;
        /* memset(hang,-1,sizeof(hang)); */
        printf("%d\n",a[n]);
    }
    return 0;
}
