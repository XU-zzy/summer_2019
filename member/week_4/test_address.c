//示例：innet 函数族的使用
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(){
    char buffer[32];
    int ret = 0;
    int host = 0;
    int network = 0;
    unsigned int address = 0;
    struct in_addr in;  //用来设置地址
    
    in.s_addr = 0;  //初始化
    
    /* 输入一个以“.”分隔的字符串形式的IP地址 */
    printf("please input your ip address\n");
    fgets(buffer,31,stdin);
    buffer[31] = '\0';

    //示例使用inet_aton()函数
    if((ret = inet_aton(buffer,&in))  == 0)
        printf("inet_aton:\tinvalid address\n");
    else
        printf("inet_aton:\t0x%x\n",in.s_addr);

    //示例使用inet_adder()函数
    if((address = inet_addr(buffer)) == INADDR_NONE){
        printf("inet_addr:\tinvalid address\n");
    }
    else{
        printf("inet_addr:\t0x%x\n",address);
    }

    //示例使用inet_network()函数
    if((address = inet_network(buffer)) == -1)
        printf("inet_work:\tinvalid address\n");
    else{
        printf("inet_work:\t0x%x\n",address);
    }

    //示例使用inet_ntoa()函数
    if(inet_ntoa(in) == NULL){
        printf("inet_ntoa:\tinvalid address\n");
    }
    else{
        printf("inet_ntoa:\t%s\n",inet_ntoa(in));
    }

    //示例使用inetlnaof()与inet_netof()函数
    host = inet_lnaof(in);
    network = inet_netof(in);
    printf("inet_lnaof:\t0x%x\n",host);
    printf("inet_natof:\t0x%x\n",network);

    in = inet_makeaddr(network,host);
    printf("inet_makeaddr:  0x%x\n",in.s_addr);
    
    return 0;
}
