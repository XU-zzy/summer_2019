#include<stdio.h>
#include<string.h>
#include<math.h>
int book[8][8];     //标记
int right[92][8];   //正确组数
int count = 0;      //控制给第几组赋值
void dfs(int lie){
    //超过界限，返回
    if(lie == 8){
        count++;
        return;
    }
    int i;
    for(i = 0;i < 8;i++){
        int k;
        if(book[i][lie] == -1){     //判断后放棋子
            book[i][lie] = lie;
            //放入right
            for(k = count;k < 92;k++){
                right[k][lie] = i + 1;
            }
            //标记棋子可覆盖的区域
            for(int m = 0;m < 8;m++){
                for(int n = 0;n < 8;n++){
                    if((m == i || n == lie || abs(m - i) == abs(n - lie)) && (book[m][n] == -1)){
                        book[m][n] = lie;
                   }
                }
            }
            //向下查找
            dfs(lie+1);
            //地图回溯
            for(int a = 0;a < 8;a++){
                for(int b = 0;b < 8;b++){
                    if(book[a][b] == lie){
                        book[a][b] = -1;
                    }
                }
            }
        }
    }
}

int main(){
    int N;
    scanf("%d",&N);
    memset(book,-1,sizeof(book));
    memset(right,-1,sizeof(right));

    dfs(0);
    int n;
    for(int i = 0;i < N;i++){
        scanf("%d",&n);
        for(int j = 0;j < 8;j++){
            printf("%d",right[n-1][j]);
        }
        printf("\n");
    }
}
