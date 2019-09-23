#include<stdio.h>
#include<algorithm>
typedef struct node {
    int start;
    int end;
}message;
bool cmp(message a,message b){
    if(a.start == b.start){
        return a.end < b.end;
    }
    else{
        return a.start < b.start;
    }
}

int main(){
    int N;
    scanf("%d",&N);

    int n;
    while(N--){
        scanf("%d",&n);
        
        int count = 0;
        message room[200+10];
        for(int i = 0;i < n;i++){
            scanf("%d %d",&room[i].start,&room[i].end);
        }

        std::sort(room,room+n,cmp);

        for(int i = 0;i < n;i++){
            j = i+1;
            if(room[i].end > room[i+1])
        }
    }
}
