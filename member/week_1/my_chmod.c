#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>    //chmod头文件
int main(int argc,char **argv){
    int mode;   //权限
    int mode_u; //所有者权限
    int mode_g; //所属用户组权限
    int mode_o; //其他用户权限
    char *path;

    if(argc != 3){
        printf("%s <modenum> <file>",argv[0]);
        exit(0);
    }
    
    mode = atoi(argv[1]);   //字符串转化为整数
    if(mode < 0 || mode > 777){
        printf("mode num wrong!");
        exit(0);
    }
    
    mode_u = mode/100;
    mode_g = (mode - mode_u*100)/10;
    mode_o = mode%10;

    mode = (mode_u * 8 * 8) + (mode_g * 8) + mode_o; //转成八进制
    path = argv[2]; //获取路径
    
    if(chmod(path,mode) == -1){
        perror("chmod error!\n");
        exit(0);
    }
    return 0;
}

