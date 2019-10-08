#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <mysql/mysql.h>                                                                                                                                            
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <termios.h>  
 

//定义参数宏
#define LOGIN                    1
#define REGISTER                 2
#define FRIEND_SEE               3
#define FRIEND_ADD               4
#define FRIEND_DEL               5
#define GROUP_SEE                6
#define GROUP_SEE_MEMBER         27
#define GROUP_CREATE             7
#define GROUP_JOIN               8
#define GROUP_QIUT               9
#define GROUP_DEL                10
#define CHAT_ONE                 11
#define CHAT_MANY                12
#define FILE_SEND_BEGIN          13
#define FILE_SEND_BEGIN_RP       14
#define FILE_SEND_STOP_RP        15
#define FILE_RECV_RE             16
#define FILE_SEND                17
#define FILE_RECV_BEGIN          18 
#define FILE_RECV_BEGIN_RP       19
#define FILE_RECV_STOP_RP        20
#define FILE_RECV                21
#define FILE_FINI_RP             22
#define EXIT                     -1
#define AGREE                    24
#define GROUP_RECORD             25
#define USERS_RECORD             26

#define SIZE_PASS_NAME   100
#define MAX_PACK_CONTIAN 100
#define MAX_CHAR         300
#define NUM_MAX_DIGIT    10
#define USER_MAX        100

#define DOWNLINE   0
#define ONLINE     1
#define BUZY       2


  //好友信息
  typedef struct  friend_info
  {
      //状态
      int statu;
      //接收到好友的信息数
      int mes_num;
      //好友名字
      char name[MAX_CHAR];
  }FRIEND_INFO;

//用户信息的群组信息
typedef struct infor_user_group{
    char group_name[20];  //群组名
    int  kind;                  //群中职位 群主 1 ，管理员 2 ，普通成员 3
    int  num;                   //群组数目
    int  group_member_num;   //群组人员数目
    char group_member_name[20][20];  //群组成员
    int  statue;
}INFOR_USER_GROUP;

  //用户信息
  typedef struct user_infor{
      char        username    [20];
      FRIEND_INFO friends     [20];
      int         friends_num;
      int         group_num;
      INFOR_USER_GROUP group[20]; // 群组信息
  }USER_INFOR;

//和服务端保持一致
//包信息
typedef struct datas{
    char     send_name[20];  //发送方
    char     recv_name[20];  //接收方
    int      send_fd;              //发送方fd
    int      recv_fd;              //接收方fd
    //time_t   time;
    char     mes[20*2];      //信息
    char     group_chat[20];   //存储群聊时发送消息的人
    int      type_2[20];                //第二标识类型
    int      mes_int;               //传数字
    char     mes_2[20][20];   //朋友
    int      mes_2_st[20];
}DATA;

typedef struct package{
    int   type;
    DATA  data;
    //INFOR_USER_GROUP group[10];   //群信息
    USER_INFOR user;         //成员信息
}PACK;












  typedef struct pthread_parameter
  {
      int a;
      int b;
  }PTHREAD_PAR;

  //聊天时消息信息
  typedef struct prinit_mes
  {
      char name[MAX_CHAR];
      char time[MAX_CHAR];
      char mes [MAX_CHAR];
  }PRINT_MES;

  typedef struct sockaddr SA;


extern USER_INFOR m_my_infor;
//-------------------------------------------------------
//函数定义

//主函数
void init();
void *deal_statu(void *arg);
void *clien_recv_thread(void *arg);
void init_clien_pthread();
int main_menu();

//功能函数
int send_login(char username_t[],char password_t[]);
int login();
int send_registe(char username_t[],char password_t[]);
void registe();
void get_status_mes();
void change_statu(PACK pack_deal_statu_t);
void upadte_friend(PACK pack_t);
//void update_friend(PACK pack_t);


void add_friend();
void del_friend();
void group_create();
void group_join();
void group_qiut();
void group_del();
void group_mes_get(USER_INFOR m_my_infor);
void get_group_member(char *group_name_t);
void update_group(PACK pack_t);
void upadte_group_member(PACK pack_t);

void send_mes_to_one();
void send_mes_to_group();
void send_mes(char mes_recv_name[],int type);

void *show_mes(void *username);
void print_mes(int id);
void send_file();
void *pthread_send_file(void *mes_t);
void send_file_send(int begin_location,char *file_path);

int file_mes_box();
void deal_file_mes(int id);
void mes_sendfile_fail(int id);
void mes_recv_requir(int id);
void mes_recvfile_fail(int id);
void *pthread_recv_file(void *par_t);

//消息记录
int mes_record();
void print_mes_record(PACK* pack_t);
void friend_history();
void group_history();

 
//界面函数
int login_menu();
void show_mes_smart(char *name  ,char *mes);
void friends_see();
void group_see();
void group_member_see();
void print_main_menu();
 
//工具函数
void my_err(const char * err_string,int line);
void sig_close(int i);
int get_file_size(char *file_name);
void file_infor_delete(int id);
int judge_same_group(char *group_name);
int judge_same_friend(char add_friend_t[]);
int get_choice(char *choice_t);
void send_pack(int type,char *send_name,char *recv_name,char *mes);
void send_pack_memcpy(int type,char *send_name,char *recv_name,char *mes);
void add_friend_agree();

#endif