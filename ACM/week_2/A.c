#include <stdio.h>

int main()
{
    int n,m;
    double min = 999999999;
    scanf("%d %d",&n,&m);
    double t,k,yuan;
    while(n--){
        scanf("%lf %lf",&k,&yuan);
        t = k/yuan; 
        if(min > t){
            min = t;
        }
    }
    printf("%.8lf",min*m);
    return 0;
}

