#include <mysql/mysql.h> 
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include "list.h"
  
#define LOGIN                    1
#define REGISTER                 2
#define FRIEND_SEE               3
#define FRIEND_ADD               4
#define FRIEND_DEL               5
#define GROUP_SEE                6  
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
#define MES_RECORD               23
#define AGREE                    24
#define EXIT                     -1
#define GROUP_RECORD             25
#define USERS_RECORD             26


 
#define FILE_STATU_RECV_ING       1
#define FILE_STATU_RECV_STOP      2
#define FILE_STATU_SEND_ING       3
#define FILE_STATU_SEND_FINI      4
 
#define SIZE_PASS_NAME  10
#define MAX_CHAR        300
#define EPOLL_MAX       200000
#define LISTENMAX       1000
#define USER_MAX        100  
#define GROUP_MAX       50
#define FILE_MAX        50
#define NUM_MAX_DIGIT   10
 
#define DOWNLINE   0
#define ONLINE     1
#define BUZY       2

#define MYSQL_NAME "zzy_chat"
#define MYSQL_USER "root"
#define MYSQL_PASSWARD "1245215743"
#define MYSQL_LINK "localhost"




int listenfd,epollfd;   //

//端口号
int PORT = 4057;
int log_file_fd;
int user_infor_fd;
int group_infor_fd;
 
//线程锁
pthread_mutex_t  mutex;
pthread_mutex_t  mutex_recv_file;
pthread_mutex_t  mutex_check_file;


//用户信息的群组信息
typedef struct infor_user_group{
    char group_name[20];  //群组名
    int  kind;                  //群中职位 群主 1 ，管理员 2 ，普通成员 3
    int  num;                   //群组数目
    int  statue;                //状态
    int         group_member_num;   //群组人员数目
    char        group_member_name[20][20];  //群组成员
}INFOR_USER_GROUP;

/////////////////////////////////////////////////////////////////////////////////////////////
 //好友信息
  typedef struct  friend_info
  {
      //状态
      int statu;
      //接收到好友的信息数
      int mes_num;
      //好友名字
      char name[20];
  }FRIEND_INFO;

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
////////////////////////////////////////////////////////////////////////////////



// 储存用户信息结构体
typedef struct infor_user 
{
    char username[MAX_CHAR];  //用户名
    char password[MAX_CHAR];          //密码
    int  statu;               //用户状态
    int  socket_id;           //用户socketID
    char friends[USER_MAX][MAX_CHAR];// 好友信息
    int  friends_num;                //  好友数量
    INFOR_USER_GROUP group[USER_MAX]; // 群组信息
    char group_name[20][20];        //加入群名字
    int group_num;                //加入群组数量
    struct infor_user *next;
    struct infor_user*prev;
}INFO_USER;

//储存群组信息结构体
typedef struct infor_group
{
    char  group_name[MAX_CHAR];  //组名
    int   member_num;            //成员数量
    char  member_name[USER_MAX][MAX_CHAR];  //成员名
    int   statue[USER_MAX]; //状态
    int   kind[USER_MAX];   //群中职位 群主 1 ，管理员 2 ，普通成员 3
    struct infor_group *next;
    struct infor_group *prev;
}INFO_GROUP;
 

//储存文件信息结构体
typedef struct file_infor
{
    char  file_name[MAX_CHAR];       //文件名
    char  file_send_name[MAX_CHAR];  //发送方
    char  file_recv_name[MAX_CHAR];  //接收方
    int   file_size;                 //文件大小
    int   file_size_now;             //文件当前大小
    int   flag ;                     //状态
}INFO_FILE;
 
 
//线程传参结构体
typedef struct pthread_parameter
{
    int a;
    char str1[SIZE_PASS_NAME];
    char str2[SIZE_PASS_NAME];
}PTHREAD_PAR;
 
typedef struct sockaddr SA;//类型转换
  

 








void *deal(void *recv_pack_t);
void *serv_send_thread(void *arg);
void *pthread_check_file(void *arg);
void init_server_pthread();
int read_users_infor();
int write_infor();

int conect_mysql_init();
void mysql_save_message(PACK *recv_pack,int flag);
void mysql_close();  

void print_send_pack();
void print_infor_group();
void print_infor_user();
void print_infor_file();
void my_err(const char * err_string,int line);

INFO_USER *find_userinfor(char username_t[]);
void signal_close(int i);
void send_pack(PACK *pack_send_pack_t);
void send_pack_memcpy_server(int type,char *send_name,char *recv_name,int sockfd1,char *mes);

int del_friend_infor(INFO_USER *p,char friend_name[]);
void del_group_from_user(char *username,char *groupname);

INFO_GROUP *find_groupinfor(char groupname_t[]);
int find_fileinfor(char *filename);
int find_user_group(char username[],char *user_group[20]);

void login(PACK *recv_pack);
void registe(PACK *recv_pack);
int registe_new_user(char username_t[],char passward[]);
void send_statu(PACK *recv_pack);
void send_group_statu(PACK *recv_pack);

void friend_add(PACK *recv_pack);
void friend_add_agree(PACK *recv_pack);
void friend_del(PACK *recv_pack);
void send_mes_to_one(PACK *recv_pack);

void group_create(PACK *recv_pack);
void group_join(PACK *recv_pack);
void group_qiut(PACK *recv_pack);
void group_del_one(int id);
void group_del(PACK *recv_pack);
void send_mes_to_group(PACK *recv_pack);



//查找函数
//查找用户信息
int user_infor_find(INFO_USER *node);
//好友查找
int user_friend_find(INFO_USER *node);
//查找群组信息
int group_infor_find(INFO_GROUP *node);
//查找群组内人员名单
int group_member_find(INFO_USER *node);



void file_recv_begin(PACK *recv_pack);
void file_recv(PACK *recv_pack);
void *file_send_send(void *file_send_begin_t);
void file_send_begin(PACK *recv_pack);
void file_send_finish(PACK *recv_pack);
void send_record(PACK *recv_pack);
  
 
//读取在线用户和群主信息
INFO_USER  *user_infor;//[USER_MAX];
int        user_num; 
INFO_GROUP   *group_infor;//[GROUP_MAX]; 
int          group_num;
INFO_FILE    m_infor_file  [FILE_MAX]; 
int          m_file_num;
 
PACK m_pack_send   [MAX_CHAR*2];
int  m_send_num;



