#include<stdio.h>
#include<malloc.h>

extern char **environ;

int main(int argc,char *argv[],char **envp){
    for(int i = 0;envp[i] != NULL;i++)
        printf("%s\n",envp[i]);
    return 0;
}
