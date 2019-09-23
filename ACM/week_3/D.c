#include<stdio.h>
int steps[8][2]={{1,0},
                {1,1},
                {0,1},
                {-1,1},
                {-1,0},
                {-1,-1},
                {0,-1},
                {1,-1}};
char map[105][105];
int n,m;

void dfs(int x,int y)
{
    map[x][y]='*';
    for(int i=0;i<8;i++)
    {
        int a = steps[i][0] + x;
        int b = steps[i][1] + y;
        if(map[a][b] == '@' && a<n && a >= 0 && b < m && b >= 0)
            dfs(a,b);
    }
}
int main()
{
    while(scanf("%d %d",&n,&m)&&n!=0&&m!=0)
    {
        int sum=0;
        getchar();

        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++)
                scanf("%c",&map[i][j]);
            getchar();
        }

        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
            if(map[i][j]=='@'){
                dfs(i,j);
                sum++;
                }
            }
        }
        printf("%d\n",sum);
    }
    return 0;

}

