#include<stdio.h>
#include<string.h>
int main(){
    char a[250],start[20],end[10],*b = "ENDOFINPUT\n";
    fgets(start,20,stdin);
    while(strcmp(start,b) != 0){
        //fgets(start,20,stdin);
        //if(strcmp(start,b) == 0){
        //    return 0;
        //}
        //printf("\nstart = %s\n",start);
        //printf("\nstrcmp = %d\n",strcmp(start,b));
        fgets(a,250,stdin);
        fgets(end,10,stdin);
        //printf("\n111\n");
        for(int i = 0;a[i] != '\n';i++){
            if(a[i] >= 'A' && a[i] <= 'E'){
                printf("%c",a[i] + 21);
            }else if(a[i] >= 'F' && a[i] <= 'Z'){
                printf("%c",a[i] - 5);
            }else{
                printf("%c",a[i]);
            }
        }
        printf("\n");
        fgets(start,20,stdin);
    }
    return 0;
}
