#include<stdio.h>
#include<unistd.h>
int main(){
    int n;
    scanf("%d",&n);
    int a[200010];
    char b[200010];
    for(int i = 0;i < n;i++){
        scanf("%d",&a[i]);
    }
    int left = 0,right = n-1,t = -1,m = 0,count = 0;
    while(left <= right){
        if(t > a[left] && t > a[right])
            break;
        if(a[left] <= a[right] ){
            if(t <= a[left]){
                t = a[left];
                b[m++] = 'L';
                left++;
                count++;
            }
            else if (t <= a[right]){
                t = a[right];
                b[m++] = 'R';
                right--;
                count++;
            }
        }
	    if(a[left] >= a[right] ) {
            if(t <= a[right]){
                t = a[right];
                b[m++] = 'R';
                right--;
                count++;  
            }
            else if (t <= a[left]){
                t = a[left];
                b[m++] = 'L';
                left++;
                count++;
            }
        }
        
        /* printf("%d\n",t); */
        /* sleep(1); */
    }
    /* b[m] = '\0'; */
    printf("%d\n",count);
    for(int i = 0;i < m;i++)
        printf("%c",b[i]);
    printf("\n");
}
