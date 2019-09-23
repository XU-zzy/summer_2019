//不对,有bug

#include <stdio.h>
#include<algorithm>
typedef struct node{
    int t_start;
    int t_end;
    int t_len;
    int t_flag;
}times;
bool cmp(times a,times b){
    if(a.t_end == b.t_end){
        return a.t_start < b.t_start;
    }
    else{
        return a.t_end < b.t_end;
    }
}

int main()
{
    int N;
    scanf("%d",&N);

    while(N--){
        int n;
        scanf("%d",&n);
        times message[100000+10];

        for(int i = 0;i < n;i++){
            scanf("%d %d",&message[i].t_start,&message[i].t_end);
            message[i].t_len = message[i].t_end - message[i].t_start;
            message[i].t_flag = 0;
        }
        std::sort(message,message+n,cmp);

        int count = 0,len = 0,max = message[n-1].t_end;
        int j = 0,end = 0;
        while(1){
            if(message[j].t_flag == 1){
                j++;
                continue;
            }

            count++;
            while(1){
                if(j == n){
                    if(end < max){
                        len += max-end;
                    }
                    break;
                }
                
                if(message[j].t_flag == 1){
                    j++;
                    continue;
                }
                
                if(message[j].t_start >= end){
                    len += message[j].t_len;
                    message[j].t_flag = 1;
                    end = message[j].t_end;
                    j++;
                }
                
            }

            j = 0,end = 0;
        }

        printf("%d %d",count,len);
    }
    return 0;
}

