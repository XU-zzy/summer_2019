#include <stdio.h>
 
int main(){
	
	int n, a;
	scanf("%d", &n);
    int k = 0,k1 = 0,k2 = 0;
	
    for(int i = 0; i < n; i++){
		scanf("%d", &a);
	    if(a%3 == 0)
	     k++;
	    else if(a%3 == 1)
	     k1++;
	    else
	     k2++;
	}
    int min = k1;
    if(k2<k1)
        min = k2;
	printf("%d\n", k/2 + min);
	
	return 0;
} 
