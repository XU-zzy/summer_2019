#include <mysql/mysql.h> 
//#include <gtk/gtk.h>
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

int listenfd,epollfd;//
short PORT = 4057;//端口号
int log_file_fd;
int user_infor_fd;
int group_infor_fd;
 
//线程锁
pthread_mutex_t  mutex;
pthread_mutex_t  mutex_recv_file;
pthread_mutex_t  mutex_check_file;



 
// 储存用户信息结构体
typedef struct infor_user 
{
    char username[MAX_CHAR];  //用户名
    char password[MAX_CHAR];          //密码
    int  statu;               //用户状态
    int  socket_id;           //用户socketID
    char friends[USER_MAX][MAX_CHAR];// 好友信息
    int  friends_num;                //  好友数量
    char group[GROUP_MAX][MAX_CHAR];  // 群组信息
    char group_num;                   //群组数量
}INFO_USER;
 
//储存群组信息结构体
typedef struct infor_group
{
    char  group_name[MAX_CHAR];  //组名
    int   member_num;            //成员数量
    char  member_name[USER_MAX][MAX_CHAR];  //成员名
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
 
//包结构体
typedef struct datas{
    char     send_name[MAX_CHAR];  //发送方
    char     recv_name[MAX_CHAR];  //接收方
    int      send_fd;              //发送方fd
    int      recv_fd;              //接收方fd
    //time_t   time;
    char     mes[MAX_CHAR*2];      //信息
    //int      *nummber[MAX_CHAR];
}DATA;
 
typedef struct package{
    int   type;
    DATA  data;
}PACK;
 
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
int read_infor();
int write_infor();
int conect_mysql_init();
void mysql_save_message(PACK *recv_pack);
void mysql_close();
void print_send_pack();
void print_infor_group();
void print_infor_user();
void print_infor_file();
void my_err(const char * err_string,int line);
int find_userinfor(char username_t[]);
void signal_close(int i);
void send_pack(PACK *pack_send_pack_t);
void send_pack_memcpy_server(int type,char *send_name,char *recv_name,int sockfd1,char *mes);
int passward_judge(int id,char *log_pass);
int judge_usename_same(char username_t[]);
void del_friend_infor(int id,char friend_name[]) ;
void del_group_from_user(char *username,char *groupname);
int find_groupinfor(char groupname_t[]);
int find_fileinfor(char *filename);
void login(PACK *recv_pack);
void registe(PACK *recv_pack);
void send_statu(PACK *recv_pack);
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
void file_recv_begin(PACK *recv_pack);
void file_recv(PACK *recv_pack);
void *file_send_send(void *file_send_begin_t);
void file_send_begin(PACK *recv_pack);
void file_send_finish(PACK *recv_pack);
void send_record(PACK *recv_pack);
  
 
//读取在线用户和群主信息
INFO_USER  user_infor  [USER_MAX];
int        m_user_num; 
INFO_GROUP   m_infor_group  [GROUP_MAX]; 
int          m_group_num;
INFO_FILE    m_infor_file  [FILE_MAX]; 
int          m_file_num;
 
PACK m_pack_send   [MAX_CHAR*2];
int  m_send_num;



