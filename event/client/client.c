#include "client.h"


USER_INFOR m_my_infor;

  //发送包
  PACK m_pack_send   [MAX_CHAR]; 
  int  m_send_num;

  //接收到的各种包
  PACK m_pack_recv_friend_see   [MAX_PACK_CONTIAN];
  PACK m_pack_recv_group_see    [MAX_PACK_CONTIAN];
  PACK m_pack_recv_chat         [MAX_PACK_CONTIAN];
  PACK m_pack_recv_send_file    [MAX_PACK_CONTIAN];
  PACK m_pack_recv_file_mes     [MAX_PACK_CONTIAN];
  PACK m_pack_recv_file         [MAX_PACK_CONTIAN];
  PACK m_pack_recv_friend_add_agree;

  int m_recv_num_friend_see;
  int m_recv_num_group_see;
  int m_recv_num_chat;
  int m_recv_num_send_file;
  int m_recv_num_file_mes;
  int m_recv_num_file;
  int m_print_mes_flag;
  
  /****************************************************/
  int m_flag_group_create;
  int m_flag_group_join ;
  int m_flag_group_del  ;
  int m_flag_print_mes;
  int m_flag_print_recorde;

  /****************************************************/
  PRINT_MES m_print_mes[10];
  int m_print_mes_num;

  //定义套接字，IP，端口
  int sockfd;

  char *IP = "192.168.1.43";
  /* char *IP = "192.168.3.210"; */
  /* char *IP = "192.168.122.1"; */
  short PORT = 4057;
  typedef struct sockaddr SA; 

  //定义锁参数
  pthread_mutex_t  mutex_local_user;
  pthread_mutex_t  mutex_recv_file;

int FLAG = 0; 

//-----------------------------------------
int send_registe_flag = -1;
int send_login_flag = -1;



//主函数
int main(int argc, char const *argv[])
  {
      
      //关闭CTRL+C 
      signal(SIGINT,sig_close);                                                                                                                                       
      //启动并连接服务器
      init();
      
      //判断是否登陆成功
      if(login_menu() == 0)
          return 0;   
      
      //开启线程
      init_clien_pthread();
      
      //主菜单
      main_menu();

      
      return 0;
  }

//创建套接字，并和服务器创建连接
void init()
{
    //创建套接字
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    
    //初始化
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);
    
    //连接
    if(connect(sockfd,(SA*)&addr,sizeof(addr))==-1){
        perror("无法连接到服务器");
        printf("客户端启动失败\n");
        exit(-1);
    }
    printf("客户端启动成功\n");
}
 
 
//更新好友状态
void upadte_friend(PACK pack_t){
    char name[20];;
    strcpy(name,m_my_infor.username);
    
    //好友数目
    m_my_infor.friends_num = pack_t.data.mes_int;

    //更新好友名称
    for(int i = 0;i < pack_t.data.mes_int;i++){
        strcpy(m_my_infor.friends[i].name,pack_t.data.mes_2[i]);
        //好友状态
        m_my_infor.friends[i].statu = pack_t.data.mes_2_st[i];
    }

    //好友数目
    m_my_infor.friends_num = pack_t.data.mes_int;

    strcpy(m_my_infor.username,name);

    return;
}

void update_friend(PACK pack_t){
    //群组数目
    m_my_infor.group_num = pack_t.data.mes_int;

}


//根据发来数据包及时更新
//好友状态
void *deal_statu(void *arg)
{
    int i;
    while(1)
    {
        pthread_mutex_lock(&mutex_local_user); 
        for(i = 1;i <= m_recv_num_friend_see;i++){   
            //printf("----====\n");
            upadte_friend(m_pack_recv_friend_see[i]);
            //change_statu(m_pack_recv_friend_see[i]);
        }
        m_recv_num_friend_see = 0;

        for(i = 1;i <= m_recv_num_group_see;i++){
            update_group(m_pack_recv_group_see[i]);
        }
        pthread_mutex_unlock(&mutex_local_user); 
        usleep(1); 
    }
}
 
 
//一直接收服务器发来的包
//并把包存储到数组当中
void *clien_recv_thread(void *arg)
{
    PACK pack_t;
    pthread_t pid_send_file;
    //始终接收包
    while(1)
    {
        //接收包
        if(recv(sockfd,&pack_t,sizeof(PACK),0) < 0){
            my_err("recv",__LINE__);
        }
        
        /* printf("\n\n*****接收包****\n");
                printf(" type    :%d\n",pack_t.type);
                printf(" from    : %s\n",pack_t.data.send_name);
                printf(" to      : %s\n",pack_t.data.recv_name);
                printf(" mes     : %s\n",pack_t.data.mes);
                printf(" recv_fd : %d\n",pack_t.data.recv_fd);
                printf(" group_chat:%s\n",pack_t.data.group_chat);
                printf(" mes_int   :%d\n", pack_t.data.mes_int);
                printf(" friends num %d\n",pack_t.user.friends_num);
                
                m_send_num-- ;
                printf(" pack left Now is:%d\n\n", m_send_num); */


        //上锁
        pthread_mutex_lock(&mutex_local_user); 
        
        //-------------------------------------------
        //统计好友消息
        for(int i=1 ;i<= m_my_infor.friends_num;i++)
        {
            if(strcmp(m_my_infor.friends[i].name,pack_t.data.send_name) == 0)
            {
                m_my_infor.friends[i].mes_num++;
                break;
            }
        }
        //--------------------------------------------        
        //对不同的包进行处理
        switch(pack_t.type)
        {
            case 0:
                break;
            /* case LOGIN:
                send_login_flag = pack_t.data.mes_int;
                printf("send_login_flag = %d\n",send_login_flag);
                break; */
            //添加好友提醒
            case AGREE:
                m_pack_recv_friend_add_agree = pack_t;
                add_friend_agree(); 
                break;
            //查看好友
            case FRIEND_SEE:
                //printf("pack = %d \n",pack_t.user.friends_num);
                m_pack_recv_friend_see[++m_recv_num_friend_see] = pack_t;
                break;
            case GROUP_SEE:
                m_pack_recv_group_see[++m_recv_num_group_see] = pack_t;

            //创建群聊
            case GROUP_CREATE:
                m_flag_group_create = pack_t.data.mes[0];
                break;
            //加入群聊
            case GROUP_JOIN:
                m_flag_group_join   = pack_t.data.mes[0];
                break;
            //解散群聊
            case GROUP_DEL:
                //m_flag_group_del    = pack_t.data.mes[0];
                break;
            //私聊
            case CHAT_ONE:
                m_pack_recv_chat[++m_recv_num_chat]  = pack_t;
                break;
            //群聊
            case CHAT_MANY:
                m_pack_recv_chat[++m_recv_num_chat]  = pack_t;
                break;
            
            //case SEND_FILE:
              //  m_pack_recv_send_file[++m_recv_num_send_file]   = pack_t;
                //break; 
            case FILE_SEND_BEGIN_RP:
                 pthread_create(&pid_send_file,NULL,pthread_send_file,(void *)pack_t.data.mes);
                break; 
            case FILE_SEND_STOP_RP:
                m_pack_recv_file_mes[++m_recv_num_file_mes]             = pack_t;
                //printf("\n\nm_pack_recv_file_mes[++m_recv_num_file_mes] 1= %s\n\n",m_pack_recv_file_mes[++m_recv_num_file_mes].data.mes); 
                break;
            case FILE_RECV_BEGIN:
                m_pack_recv_file_mes[++m_recv_num_file_mes]             = pack_t;
                /* printf("pack = %s",pack_t.data.mes+NUM_MAX_DIGIT); */
                /* printf("\n\nm_pack_recv_file_mes[++m_recv_num_file_mes] 2= %s\n\n",m_pack_recv_file_mes[++m_recv_num_file_mes].data.mes); */
                break;
           

            //接收文件
            case FILE_RECV:
                pthread_mutex_lock(&mutex_recv_file); 
                m_pack_recv_file[++m_recv_num_file]                     = pack_t;                
                //printf("\n\nm_pack_recv_file_mes[++m_recv_num_file_mes] 3= %s\n\n",m_pack_recv_file_mes[++m_recv_num_file_mes].data.mes); 
                pthread_mutex_unlock(&mutex_recv_file);
                break; 
            case FILE_RECV_STOP_RP:
                m_pack_recv_file_mes[++m_recv_num_file_mes]             = pack_t;
                //printf("\n\nm_pack_recv_file_mes[++m_recv_num_file_mes] 4= %s\n\n",m_pack_recv_file_mes[++m_recv_num_file_mes].data.mes); 
                break;
            //消息记录
            case GROUP_RECORD:
            case USERS_RECORD:
                {
                    PACK *pack_record;
                    pack_record = (PACK *)malloc(sizeof(PACK));
                    memcpy(pack_record,&pack_t,sizeof(PACK));
                    print_mes_record(pack_record);
                    break;
                }
        }
        //解锁
        pthread_mutex_unlock(&mutex_local_user); 
        usleep(1); 
    }
}
 
 
//开启线程
void init_clien_pthread()
{
    pthread_t pid_deal_statu,pid_recv,pid_recv_file;
    //更新状态收包处理线程
    pthread_create(&pid_deal_statu,NULL,deal_statu,NULL);
    //实时接收包线程
    pthread_create(&pid_recv,NULL,clien_recv_thread,NULL);
} 
 
 
//主菜单
int main_menu()
{
    char choice_t[100];
    int chioce;
    do
    {
        //更新信息
        get_status_mes();
        //printf("pack num_chat:%d\n", m_recv_num_chat);
        //打印主菜单
        print_main_menu();
        scanf("%s",choice_t);
        chioce = get_choice(choice_t);
        switch(chioce)
        {  
            case 1:
                //查看好友
                friends_see();
                break;
            case 2:
                //添加好友
                add_friend();
                break;
            case 3:
                //删除好友
                del_friend();
                break;
            case 4:
                //查看群组
                group_see();
                break;
            case 5:
                //创建群组
                group_create();
                break;
            case 6:
                //加入群组
                group_join();
                break;
            case 7:
                //退出群组
                group_qiut();
                break;
            case 8:
                //解散群组
                group_del();
                break;
            case 9:
                //私聊
                send_mes_to_one();
                break;
            case 10:
                //群聊
                send_mes_to_group();
                break;
            case 11:
                //发送文件
                send_file();
                break;
            case 12:
                //文件消息盒子
                file_mes_box();
                break;
            case 13:
                //消息记录
                mes_record();
            default:
                break;
        }
    }while(chioce!=0);
    return 0;
}



//界面函数--------------------------------------------------------------------------------------------------------------------------
//登陆菜单
int login_menu(){
    char choice_t[100];
    int chioce;
    do{
        printf("\n*******************************\n");
        printf("        [1].登录            \n");
        printf("        [2].注册            \n");
        printf("        [0].退出                \n");
        printf("*******************************\n");
        printf("你的选择：");
        scanf("%s",choice_t);
        chioce = get_choice(choice_t);
        switch(chioce){  
            //登录
            case 1:
                if(login() == 1)
                    return 1;
                break;
            //注册
            case 2:
                registe();
                break;
            default:
                break;
        }
    }while(chioce!=0);
    return 0;
}

//主菜单 
void print_main_menu()
{
    printf("\n********************************* \n");
    printf("        [1]显示好友       \n");
    printf("        [2]添加好友       \n");
    printf("        [3]删除好友       \n");
    printf("        [4]显示群组         \n");
    printf("        [5]创建群组         \n");
    printf("        [6]加入群组         \n");
    printf("        [7]退出群组         \n");
    printf("        [8]解散群组         \n");
    printf("        [9]私聊        \n");
    printf("        [10]群聊      \n");
    printf("        [11]发文件          \n");
    printf("        [12]文件消息盒子 %d \n",m_recv_num_file_mes);
    printf("        [13]查看历史消息\n");
    printf("        [0]退出                 \n");
    printf("*********************************\n");
    printf("选择：");
}


//功能函数--------------------------------------------------------------------------------------------------------------------------
//向服务端发送登陆信息
int send_login(char username_t[],char password_t[])
{
    PACK recv_login_t;
    //printf("1=======\n");
    //发送包
    send_pack(LOGIN,username_t,"server",password_t);
    
    //printf("2=======\n");
    
    if(recv(sockfd,&recv_login_t,sizeof(PACK),0) < 0){
        my_err("recv",__LINE__);
    }
    //printf("3=======\n");
    
    printf("wait......\n");
    //sleep(1);
    //接收服务器返回的登录消息
    int login_judge_flag = recv_login_t.data.mes_int;
    //printf("---------recv_login_t.data.mes_int = %d\n",login_judge_flag);
    return login_judge_flag;
}
 

//根据输入信息向客户端发送登陆请求
//并根据返回内容，提示用户
int login()
{
    int login_flag = 0;
    char username_t [MAX_CHAR];
    char password_t [MAX_CHAR];
    
    printf("请输入用户名:\n");
    scanf("%s",username_t);
    printf("请输入密码：\n");
    scanf("%s",password_t);
    //printf("0=======\n");
    int flag = send_login(username_t,password_t);
    
    //printf("--------------------------------------%d\n",flag);

    if(flag ==  2){
        printf("用户未注册.\n");
        return 0;
    }   
    if(flag ==  3 ){
        printf("用户已登录 .\n");
        return 0;
    }  
    if(flag ==  0) {
        printf("密码不匹配.\n");
        return 0;
    }
    
    //初始化当前登录用户名
    strcpy(m_my_infor.username,username_t);
    printf("登陆成功\n");
    return 1;
}
 
//向服务端发送注册信息
//成功返回1
int send_registe(char username_t[],char password_t[])
{
    PACK recv_registe_t;
    
    send_pack(REGISTER,username_t,"server",password_t);
    
    if(recv(sockfd,&recv_registe_t,sizeof(PACK),0) < 0){
        my_err("recv",__LINE__);
    }
    
    printf("wait......\n");
    sleep(1);

    return recv_registe_t.data.mes_int;
}
 
//根据输入信息向客户端发送注册请求
//并根据返回内容，提示用户
void registe()
{
    int flag = 0;
    flag = REGISTER;
    char username_t[MAX_CHAR];
    char password_t[MAX_CHAR];
    
    printf("注册账号:\n");
    scanf("%s",username_t);
    printf("注册密码:\n");
    scanf("%s",password_t);
    if(send_registe(username_t,password_t))
        printf("成功!\n");
    else 
        printf("姓名重复\n");
}  
 
//向服务端请求更新好友状态
void get_status_mes()
{
    PACK pack_friend_see;
    pack_friend_see.type = FRIEND_SEE;
    
    strcpy(pack_friend_see.data.send_name,m_my_infor.username);
    printf("friend mes send name : %s\n",m_my_infor.username);
    strcpy(pack_friend_see.data.recv_name,"server");
    memset(pack_friend_see.data.mes,0,sizeof(pack_friend_see.data.mes));
    
    if(send(sockfd,&pack_friend_see,sizeof(PACK),0) < 0){
        my_err("friend mes send\n",__LINE__);
    }

    PACK pack_group_see;
    pack_group_see.type = GROUP_SEE;

    strcpy(pack_group_see.data.send_name,m_my_infor.username);
    printf("group mes send name %s\n",m_my_infor.username);
    strcpy(pack_group_see.data.recv_name,"server");
    memset(pack_group_see.data.mes,0,sizeof(pack_group_see.data.mes));

    if(send(sockfd,&pack_group_see,sizeof(PACK),0) < 0){
        my_err("group mes send\n",__LINE__);
    }
}
 
/* //根据服务端发送来的包，利用字符串解析，更新当前好友状态
void change_statu(PACK pack_deal_statu_t)
{
    int count = 0;
    m_my_infor.friends_num=pack_deal_statu_t.data.mes[count++];
    
    printf("SAdasdas\n");
    //更新好友信息
    for(int i=1; i <= m_my_infor.friends_num ;i++)
    {
        for(int j=0;j<SIZE_PASS_NAME;j++)
        {
            if(j == 0)   
                m_my_infor.friends[i].statu = pack_deal_statu_t.data.mes[count+j] - 48;
            else
                m_my_infor.friends[i].name[j-1] = pack_deal_statu_t.data.mes[count+j];
        }
        count += SIZE_PASS_NAME;
    }
    
    //更新群组信息
    m_my_infor.group_num=pack_deal_statu_t.data.mes[count++];
    for(int i=1 ;i <= m_my_infor.group_num ;i++)
    {
        for(int j=0;j<SIZE_PASS_NAME;j++)
        {
            //m_my_infor.  group[i][j] = pack_deal_statu_t.data.mes[count+j];
        }
        count += SIZE_PASS_NAME;
    }
} */
//定义锁参数
extern pthread_mutex_t  mutex_local_user;
extern pthread_mutex_t  mutex_recv_file;

//好友列表查看
void friends_see()
{
    pthread_mutex_lock(&mutex_local_user);
    printf("***********好友列表*************\n");
    printf("\n---%d\n",m_my_infor.friends_num);
    for(int i=0 ;i < m_my_infor.friends_num ;i++){
        
        switch(m_my_infor.friends[i].statu){
           case ONLINE:
                printf("  ID[%d]:       %s [ONLINE] ", i,m_my_infor.friends[i].name);
                //好友消息
                if(m_my_infor.friends[i].mes_num)
                    printf("%d messages\n", m_my_infor.friends[i].mes_num);
                else 
                    printf("\n");
                break;
           case DOWNLINE:
                //printf("\n---%d\n",m_my_infor.friends_num);
                printf("\n  ID[%d]:     %s [DOWNLINE] ", i,m_my_infor.friends[i].name);
                if(m_my_infor.friends[i].mes_num)
                    printf("%d messages\n", m_my_infor.friends[i].mes_num);
                else 
                    printf("\n");
                break;
        }
    }
    printf("\n\n");
    printf("********************************\n");
    pthread_mutex_unlock(&mutex_local_user);  
}

//添加好友
void add_friend()
{
    char add_friend_t[MAX_CHAR];
    
    printf("请输入要添加的好友名称:\n");
    scanf("%s",add_friend_t);
    getchar();
    if(strcmp(add_friend_t,m_my_infor.username) == 0){
        printf("不能添加自己!\n");
        return;
    }

    int id;
    //判断是否已经添加过该好友
    if((id = judge_same_friend(add_friend_t))!= -1)
    {
        printf("\nm_my = %s\n",m_my_infor.friends[id]);
        printf("你已经添加过他!\n");
        return ;
    }
    //printf("m_my_infor.username:%s\n", m_my_infor.username);
    send_pack(FRIEND_ADD,m_my_infor.username,"server",add_friend_t);
    //get_status_mes();
}

//是否同意添加好友请求
void add_friend_agree(){
    int choice;
    printf("\n----------message----------\n");
    printf("%s 想要添加你为好友！\n",m_pack_recv_friend_add_agree.data.mes);
    do{
        printf("[1]同意\t[2]不同意\n");

        //scanf("%d",&choice);
        fflush(stdin);
        printf("请确认:\n");
        scanf("%d",&choice);
        
        if(choice == 2){
            printf("拒绝添加！\n");
            return;
        }
        else if(choice == 1){
            send_pack(AGREE,m_my_infor.username,"server",m_pack_recv_friend_add_agree.data.mes);
            return;
        }
    
    }while(1);
}

//删除好友
void del_friend()
{
    char del_friend_t[MAX_CHAR];
    printf("请输入要删除好友名称:\n");
    scanf("%s",del_friend_t);
    
    //判断是否添加过该好友
    if(judge_same_friend(del_friend_t) == -1 ){
        printf("你没有这个好友!\n");
        return ;
    }
    
    //发送包
    send_pack(FRIEND_DEL,m_my_infor.username,"server",del_friend_t);
    //得到状态信息
    //get_status_mes();
}


//群组信息查看
void group_see()
{
    pthread_mutex_lock(&mutex_local_user);
    
    //获取群信息
    //group_mes_get(m_my_infor);
    
    printf("***********群组列表*************  \n");
    //int i;
    for(int i=1 ;i<=m_my_infor.group_num ;i++){
        printf("  ID[%d]:       %s\n", i,m_my_infor.group[i]);
    }
    printf("\n\n");
    printf("*************************************** \n");
    int choice;
    do{
        printf("[1]查看群成员\t[2]退出\n");
        fflush(stdin);
        scanf("%d",&choice);

        if(choice == 1){
            //group_member_see();
        }

        if(choice != 1 && choice != 2){
            printf("请重新输入\n");
        }

    }while(choice != 2 && choice != 1);
   
    pthread_mutex_unlock(&mutex_local_user);  
}

/* //查看群成员列表
void group_member_see(){
    int choice;
    do{
        printf("请输入你要查看的群组的序号（输入0退出）：");
        fflush(stdin);
        scanf("%d",&choice);
        
        if(choice != 0){
            get_group_member(choice);

            printf("正在查询.....\n");
            sleep(2);
            printf("\n************群成员列表**************\n");

            for(int i = 1;i <= m_my_infor.group_member_num;i++){
                printf("ID[%d]\t\t%s\n",i,m_my_infor.group_member_name[choice][i]);
            }

            printf("\n**************************************\n");
        }

    }while(choice != 0);
    return;
} */

/* void get_group_member(int n){

} */
 


















//创建群组
void group_create()
{
    char group_name[MAX_CHAR];
    
    printf("请输入要建立的群组名称:\n");
    scanf("%s",group_name);
    send_pack(GROUP_CREATE,m_my_infor.username,"server",group_name);
    
    //等待服务端回复后，跳出循环
    while(!m_flag_group_create);
    
    if(m_flag_group_create == 2) 
        printf("建立成功!\n");
    else if(m_flag_group_create == 1)
        printf("与其他群组重名!\n");
    
    m_flag_group_create = 0;
}
 
 
//加入群组
void group_join()
{
    char group_name[MAX_CHAR];
    printf("输入要加入的群组名称:\n");
    scanf("%s",group_name);
    
    //判断是否已经加入该群
    for(int i=1;i <= m_my_infor.group_num ;i++)
    {
        if(strcmp(m_my_infor.group[i].group_name,group_name) == 0)
        {
            printf("你已经加入该群组!\n");
            return ;
        }
    }
 
    send_pack(GROUP_JOIN,m_my_infor.username,"server",group_name);
    
    //等待服务器回复后，跳出循环
    while(!m_flag_group_join);
    
    if(m_flag_group_join == 2) 
        printf("成功!\n");
    else if(m_flag_group_join == 1)
        printf("没有群组叫 %s\n",group_name);
    
    m_flag_group_join = 0;
}
 
//退出群组
void group_qiut()
{
    char group_name[MAX_CHAR];
    
    printf("请输入要退出的群组名称:\n");
    scanf("%s",group_name);
    
    //判断是否添加过该群组
    for(int i=1;i <= m_my_infor.group_num ;i++)
    {
        if(strcmp(m_my_infor.group[i].group_name,group_name) == 0)
        {
            send_pack(GROUP_QIUT,m_my_infor.username,"server",group_name);
            printf("退出 %s 成功!\n",group_name);
            return ;
        }
    }
    
    printf("你没加入过这个群组!\n");
}
 
//解散群
void group_del()
{
    char group_name[MAX_CHAR];
    
    printf("请输入要解散的群名称:\n");
    scanf("%s",group_name);
    
    for(int i=1;i <= m_my_infor.group_num ;i++)
    {
        if(strcmp(m_my_infor.group[i].group_name,group_name) == 0)
        {
            send_pack(GROUP_DEL,m_my_infor.username,"server",group_name);
            
            //等待服务端回复
            while(!m_flag_group_del);
            //printf("m_flag_group_del=%d\n", m_flag_group_del);
            
            if(m_flag_group_del == 2) 
                printf("解散成功!\n");
            else if(m_flag_group_del == 1)
                printf("你不是 %s 的群主！无法解散\n",group_name);
           
            return ;
        }
    }
    
    printf("你没有在这个群组中!\n");
}
 
//================================================================================================================================
 
//私聊
void send_mes_to_one()
{
    pthread_t pid;
    int id;
    char mes_recv_name[MAX_CHAR];
    
    //打印好友列表
    friends_see();
 
    printf("你想私聊的对象：\n");
    scanf("%s",mes_recv_name);
    
    if (id=judge_same_friend(mes_recv_name)){
        printf("你没有添加这个好友 !%s\n",mes_recv_name);
        return ;
    }
    
    printf("****************************消息*********************************\n");
    
    //打印消息标志
    m_flag_print_mes = 1;
    
    //消息数目
    m_my_infor.friends[id].mes_num = 0;
    
    //开启线程显示信息
    pthread_create(&pid,NULL,show_mes,(void *)mes_recv_name);
    
    //发送消息
    send_mes(mes_recv_name,CHAT_ONE);
}
 
//群聊
void send_mes_to_group()
{
    pthread_t pid;
    char mes_recv_group_name[MAX_CHAR];
    //group_see();
    printf("请输入群聊名称\n");
    scanf("%s",mes_recv_group_name);
    if (!judge_same_group(mes_recv_group_name))
    {
        printf("你没有加入这个群聊 !%s\n",mes_recv_group_name);
        return ;
    }
 
    m_flag_print_mes = 1;
    printf("****************************群聊*********************************\n");
    
    //开启线程显示信息
    pthread_create(&pid,NULL,show_mes,(void *)mes_recv_group_name);
    
    //发送消息
    send_mes(mes_recv_group_name,CHAT_MANY);

}
 
 //显示聊天信息
void show_mes_smart(char *name  ,char *mes)
{
    time_t timep;
    int number = 10;
    char time1[100];
    int len ; 
    //时间
    time (&timep);
    strcpy(time1,ctime(&timep));
    len = strlen(time1);
    time1[len-5] = '\0'; 

    //确认要打印的聊天信息发送方
    if(m_print_mes_num == number)  {
        for(int i=1;i<=9 ;i++)
            m_print_mes[i] = m_print_mes[i+1];
        strcpy(m_print_mes[number].name,name);
        strcpy(m_print_mes[number].mes,mes);
    }else{
         strcpy(m_print_mes[++m_print_mes_num].name,name);
         strcpy(m_print_mes[m_print_mes_num].mes,mes);
    }
 
    //打印聊天信息
    int i = m_print_mes_num;
    if(strcmp(m_print_mes[i].name,m_my_infor.username) == 0){
            printf("%s\t%s\n",m_print_mes[i].name,time1+10);
            printf("%s\n", m_print_mes[i].mes);
    }else{
            printf("%s\t%s\n",m_print_mes[i].name,time1+10);
            printf("%s\n", m_print_mes[i].mes);
    }

    //刷缓存区
    fflush(stdout);
}


//接收用户输入并发送信息到客户端
void send_mes(char mes_recv_name[],int type)
{
    /* PACK pack_send_mes; */
    char mes[MAX_CHAR];
    time_t timep;
    getchar();
    printf("****************************消息*********************************\n");
    while(1){   
        time(&timep);
        memset(mes,0,sizeof(mes));
        fflush(stdout);
        //if(type == CHAT_ONE)
            // printf("%s->",m_my_infor.username);
        fgets(mes,MAX_CHAR,stdin);
        while(mes[0] == 10){
            fflush(stdout);
            fgets(mes,MAX_CHAR,stdin);
        }

        mes[strlen(mes)-1] = '\0';

        //当用户输入quit时退出
        if(strcmp(mes,"quit\0") == 0)
            break;
 
        //输入的的同时，输出信息
        show_mes_smart(m_my_infor.username ,mes);
        
        //printf("\t%s\n%s\n", m_my_infor.username,ctime(&timep),mes);
        //发送给客户端
        send_pack(type,m_my_infor.username,mes_recv_name,mes);
    }
    m_flag_print_mes = EXIT;
}
 
//在聊天的同时启动,启动线程读取，存储区域的消息，并显示出来
void *show_mes(void *username)
{
    int id;
    char *user_name = (char *)username;
    while(1)
    {
        //如果退出聊天，就不再显示
        if (m_flag_print_mes == EXIT)
            break;
        pthread_mutex_lock(&mutex_local_user); 
        id = 0;
 
        //检索信息
        for(int i = 1 ;i <= m_recv_num_chat; i++){
            if(strcmp(m_pack_recv_chat[i].data.send_name,user_name) == 0){
                id = i;
 
                //输出信息
                print_mes(id);
                m_recv_num_chat--;
                for(int j = id; j <= m_recv_num_chat && m_recv_num_chat ;j++){
                    m_pack_recv_chat[j]  =  m_pack_recv_chat[j+1];
                }
                break;
            }
        }
        
        pthread_mutex_unlock(&mutex_local_user); 
        usleep(1);    
    }
}
 
//根据寻找到的包把信息输出
void print_mes(int id){
    char group_print_name[MAX_CHAR];

    //确认发送人名称
    memset(group_print_name,0,sizeof(group_print_name));
    
    //判断聊天类型,私聊还是群聊
    if(m_pack_recv_chat[id].type == CHAT_ONE){
        show_mes_smart(m_pack_recv_chat[id].data.send_name,m_pack_recv_chat[id].data.mes);
    }else{    
        //群聊下依次存入发消息的人的名称
        /* for(int i=0;i<SIZE_PASS_NAME;i++){
            group_print_name[i] = m_pack_recv_chat[id].data.mes[i];
        } */
        strcpy(group_print_name,m_pack_recv_chat[id].data.group_chat);
        show_mes_smart(group_print_name,m_pack_recv_chat[id].data.mes);
    }
}
 
 
 
//=============================================================================================================================
//向服务端请求发送文件，并将文件的大小发送给服务端
 
void send_file()
{
    char  recv_name[MAX_CHAR];
    char  file_path[MAX_CHAR];
    int   file_size_t;
    char  mes_t[MAX_CHAR];
    friends_see();
    printf("请输入收件人:\n");
    scanf("%s",recv_name);
 
    int id = judge_same_friend(recv_name);
    if(id == 0)
    {
        printf("你没有这个好友!\n");
        return ;
    }
    printf("请输入文件名 :\n");
    scanf("%s",file_path); 
    
    //得到文件的大小
    file_size_t = get_file_size(file_path);
    printf("文件大小 :%d\n", file_size_t);
 
    if(file_size_t == 0)
    {
        printf("文件大小为0\n");
        return ;
    }
    //字符串分析
    int digit = 0;
    while(file_size_t != 0)
    {   
        mes_t[digit++] = file_size_t%10;
        file_size_t /= 10;
    }
    mes_t[digit]  = -1;
   
    /*for(int i=0 ;i< SIZE_PASS_NAME ;i++)
    {
        mes_t[NUM_MAX_DIGIT+i] = file_path[i];
    }*/


    int i; 
    for(i = 0;i < SIZE_PASS_NAME && file_path[i] != '\0';i++){
        mes_t[NUM_MAX_DIGIT+i] = file_path[i];
    }
    mes_t[NUM_MAX_DIGIT + i] = '\0';


    //发送请求
    send_pack_memcpy(FILE_SEND_BEGIN,m_my_infor.username,recv_name,mes_t);
}
 
 
 
//当接收到允许发送的消息时，开启线程发送文件
void *pthread_send_file(void *mes_t)
{
    char *mes = (char *)mes_t;
    int begin_location = 0;
    char file_name[MAX_CHAR];
    printf("\nfunction:pthread_send_file \n");
    
    //解析到服务端已接收文件大小
    for(int i=0 ;i<NUM_MAX_DIGIT ;i++)
    {
        if(mes[i] == -1)  
            break;
        int t1 = 1;
        for(int l=0;l<i;l++)
            t1*=10;
        begin_location += (int)mes[i]*t1;
 
    }
    printf("mes:%s NUM_MAX_DIGIT = %d\n",mes,NUM_MAX_DIGIT);
    strcpy(file_name,mes+NUM_MAX_DIGIT);
    send_file_send(begin_location,file_name);

}
 
 
//从起始位置向服务端发送文件
void send_file_send(int begin_location,char *file_path)
{
    int fd;
    int length;
    int file_size;
    int sum = begin_location;
    char mes[MAX_CHAR*2];
    printf("****************************发送文件*********************************\n");
    printf("\n\n正在发送文件......\n");
    
    /* printf("%s",file_path); */
    //打开文件
    if((fd = open(file_path,O_RDONLY)) == -1)
    {
        printf("%s\n",file_path);
        my_err("open",__LINE__);
        return ;
    }
    file_size=lseek(fd, 0, SEEK_END);
    
    printf("文件大小=%d",file_size);
    //文件内部指针移动
    lseek(fd ,begin_location ,SEEK_SET);
 
    bzero(mes, MAX_CHAR*2); 
 
    // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止 
    while((length = read(fd  ,mes+NUM_MAX_DIGIT ,(MAX_CHAR*2 - NUM_MAX_DIGIT))) > 0) 
    {
        sum += length;
      //  printf("length = %d\n", length);
        int digit = 0;
        while(length != 0)
        {   
            mes[digit++] = length%10;
            length /= 10;
        }
        mes[digit]  = -1;
        printf("正在发送。。。\n");
        send_pack_memcpy(FILE_SEND,m_my_infor.username,file_path,mes);
        
        if(sum == file_size)  
            break;
        bzero(mes, MAX_CHAR*2); 
        usleep(1000);
        fflush(stdout);
    } 
    // 关闭文件 
    close(fd);
    printf("send finished!!\n");
    print_main_menu();
}

//显示文件处理消息，并根据提供选择
int file_mes_box()
{
    char choice_t[100];
    int chioce;
    do
    {
        get_status_mes();
        printf("****************************文件消息盒子*********************************\n");
        for(int i = 1; i <= m_recv_num_file_mes;i++)
        {
            if(m_pack_recv_file_mes[i].type == FILE_SEND_STOP_RP)
                printf("send file %s filed      \n",m_pack_recv_file_mes[i].data.send_name);
            if(m_pack_recv_file_mes[i].type == FILE_RECV_BEGIN)
                printf("%s send file %s to you   \n", m_pack_recv_file_mes[i].data.send_name,m_pack_recv_file_mes[i].data.mes+SIZE_PASS_NAME);
            if(m_pack_recv_file_mes[i].type == FILE_RECV_STOP_RP)
                printf("   recv file %s filed       \n", m_pack_recv_file_mes[i].data.mes+NUM_MAX_DIGIT);
        }
                printf("             0.exit                               \n");
        printf("*************************************************************************\n");
        printf("choice：");
        scanf("%s",choice_t);
        chioce = get_choice(choice_t);
        //进行处理
        if(chioce != -1)   
            deal_file_mes(chioce);
 
    }while(chioce!=0);
    return 0;
}
 




//消息记录==============================================================================================================================
int mes_record(){
    int choice;
    do{
        printf("\n******************************消息记录*************************************\n");
        printf("\t\t\t[1]好友消息\n");
        printf("\t\t\t[2]群消息\n");
        printf("\t\t\t[0]退出");
        printf("\n***************************************************************************\n");
        fflush(stdin);
        scanf("%d",&choice);
        switch (choice)
        {
         case 1:
            friend_history();
            break; 
        case 2:
            //printf("\n================聊天记录================\n");
            group_history();
            break;
        default:
            break;
        }

    }while(choice != 0);

    return 0;
}


//群聊聊天记录
void group_history(){
    char group_name[MAX_CHAR];
    
    //获取群信息
    //group_mes_get(m_my_infor);
    
    do{
        printf("***********群组列表*************  \n");
        //int i;
        for(int i=1 ;i<=m_my_infor.group_num ;i++){
            printf("  ID[%d]:       %s", i,m_my_infor.group[i]);
        }
        printf("\n\n");
        printf("*************************************** \n");
        printf("请输入要查看的群的名称(按 q 退出)：\n");

        fflush(stdin);
        scanf("%s",group_name);
        
        //退出
        if(strcmp(group_name,"q\0") == 0){
            return;
        }

        if(judge_same_group(group_name) == 1){
            printf("找到群组！\n");
            break;
        }else{
            printf("你没有加入这个群聊 !%s\n",group_name);
            printf("请重新输入！\n");
            continue;
        }
    
    }while(strcmp(group_name,"q\0") != 0);
        
        send_pack(GROUP_RECORD,m_my_infor.username,"server",group_name);

        printf("请稍等......\n");
        /* while(FLAG == 1){
            FLAG = 0;
            return;
        } */
        //sleep(1);
        //print_mes_record();
        return ;
}

//好友聊天记录
void friend_history(){
    char friend_name[MAX_CHAR];
    int fid;
    do{
        friends_see();
        printf("请输入要查看的好友的名称(按 q 退出)：\n");

        fflush(stdin);
        scanf("%s",friend_name);
        /* 
        break;
        //退出
        if(strcmp(friend_name,"q\0") == 0){
            return;
        }

        if(judge_same_friend(friend_name) == -1){
            for(int i = 1;i <= m_my_infor.friends_num;i++){
                //if(strcmp(m_my_infor.friends[i].name,add_friend_t) == 0)
                printf("\n%d = %s\n",i,m_my_infor.friends[i].);
            //return i;
            }
            
            
            printf("你没有这位好友 !%s\n",friend_name);
            printf("请重新输入！\n");
            continue;
        }else{
            printf("找到好友！\n");
            break;
        }
 */
    }while(strcmp(friend_name,"q\0") != 0);

    send_pack(USERS_RECORD,m_my_infor.username,"server",friend_name);

    printf("请稍等......\n");
    
    return;
}


void print_mes_record(PACK* pack_t){
    /* printf("\n================聊天记录================\n"); */
    printf("%s:\n",pack_t->data.group_chat);
    printf("%s\n",pack_t->data.mes);
    return;
}



 
















//===========================================================================================================================
//对文件信息进行分类处理
void deal_file_mes(int id)
{
    if(m_pack_recv_file_mes[id].type == FILE_SEND_STOP_RP)
    {
        mes_sendfile_fail(id);
    }
    else if(m_pack_recv_file_mes[id].type == FILE_RECV_BEGIN)
    {
        mes_recv_requir(id);
    }/*else if(m_pack_recv_file_mes[id].type == FILE_RECV_STOP_RP)
    {
        mes_recvfile_fail(id);
    }*/
}
 
 
//处理文件上传失败，并询问是否重发
//进行断点续传
void mes_sendfile_fail(int id)
{
    char chioce[10];
    //解析已经上传的字节数
    int begin_location = 0;
    for(int i=0 ;i<NUM_MAX_DIGIT ;i++)
    {
        if( m_pack_recv_file_mes[id].data.mes[i] == -1)  
            break;
        printf("%d\n\n",m_pack_recv_file_mes[id].data.mes[i]);
        int t1 = 1;
        for(int l=0;l<i;l++)
            t1*=10;
        begin_location += (int)m_pack_recv_file_mes[id].data.mes[i] * t1;
 
    }
 
 
    //询问是否重传
    int file_size_t = get_file_size(m_pack_recv_file_mes[id].data.send_name);
    printf("the file %s send failed ,have sended %d%%,do you want send again?\n", m_pack_recv_file_mes[id].data.send_name,(int)((double)begin_location/file_size_t*100));
    printf("y/n :");
    scanf("%s",chioce);
    
 
    if(chioce[0] != 'Y' && chioce[0] != 'y')
    {
        file_infor_delete(id);
        return ;
    }
    
    //进行重传
    send_file_send(begin_location,m_pack_recv_file_mes[id].data.send_name);
    file_infor_delete(id);
}
 
//处理下载文件请求，询问是否接收文件
void mes_recv_requir(int id)
{
    pthread_t  pid_recv_file;
    char choice[10];
    int len ;
    int fd;
    char mes_t[MAX_CHAR];
    int file_size = 0;
    char file_name[SIZE_PASS_NAME]; 
    
    PTHREAD_PAR * par_t = (PTHREAD_PAR *)malloc(sizeof(PTHREAD_PAR));
    //解析文件大小
    for(int i=0 ;i<NUM_MAX_DIGIT ;i++)
    {
        if(m_pack_recv_file_mes[id].data.mes[i] == -1)  
            break;
        int t1 = 1;
        for(int l=0;l<i;l++)
            t1*=10;
        file_size += (int)m_pack_recv_file_mes[id].data.mes[i]*t1;
 
    }
     //for(int i=0 ;i<=50;i++)
     //    printf("%d\n", m_pack_recv_file_mes[id].data.mes[i]);
     //printf("%s\n", m_pack_recv_file_mes[id].data.mes+NUM_MAX_DIGIT);
    strcpy(file_name,m_pack_recv_file_mes[id].data.mes+NUM_MAX_DIGIT);
 
    
    printf("是否接收 %s 送来的 %s 大小(%db)？ \n", m_pack_recv_file_mes[id].data.send_name,file_name,file_size);
    printf("[1]接收\t[2]拒绝\n");
    printf("输入你的选择：");
    scanf("%s", choice);
    
    if(choice[0] == '2')
    {
        file_infor_delete(id);
        return ;
    }
 
 
    //建立文件
    if((fd = open(file_name,O_WRONLY | O_CREAT , S_IRUSR | S_IWUSR)) < 0)
    {
        my_err("open",__LINE__);
        return ;
    }
    len = lseek(fd, 0, SEEK_END);
    close(fd);
 
    par_t->a = file_size;
    par_t->b = len;
    
    int digit = 0;
    while(len != 0)
    {   
        mes_t[digit++] = len%10;
        len /= 10;
    }
    mes_t[digit]  = -1;
    //发送同意同意
    send_pack_memcpy(FILE_SEND_BEGIN_RP ,m_my_infor.username ,file_name ,mes_t);
    //开启线程接收文件
    pthread_create(&pid_recv_file,NULL,pthread_recv_file,(void *)par_t);
 
    file_infor_delete(id);
}
 

//----------------------------------------------------------------------------------------------------------------- 

//接收文件线程，
//从存储接收包的地方检索到信息
//并写入文件
//当文件写入完成，关闭线程
void *pthread_recv_file(void *par_t)
{
    PTHREAD_PAR * pthread_par  = (PTHREAD_PAR * )par_t;
    int file_size              = pthread_par->a ;
    int begin_location_server  = pthread_par->b;
    int sum                    = begin_location_server; 
    while(1)
    {
        pthread_mutex_lock(&mutex_recv_file); 
        int  fd;
        char file_name[MAX_CHAR];
        for(int i=1;i<=m_recv_num_file ;i++)
        {
            
            int  len = 0;
            for(int j=0 ;j<NUM_MAX_DIGIT ;j++)
            {
                if(m_pack_recv_file[i].data.mes[j] == -1)  
                    break;
                int t1 = 1;
                for(int l=0;l<j;l++)
                    t1*=10;
                len += (int)m_pack_recv_file[i].data.mes[j]*t1;
 
            }
 
            strcpy(file_name,m_pack_recv_file[i].data.send_name);
            if((fd = open(file_name,O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) < 0)
            {
                my_err("open",__LINE__);
                return NULL;
            }
 
            if(write(fd,m_pack_recv_file[i].data.mes + NUM_MAX_DIGIT,len) < 0)
                my_err("write",__LINE__);
            // 关闭文件 
            close(fd);
            sum += len;
            m_recv_num_file = 0;
            //文件接收完成，输出提示信息，跳出线程
            if(sum >= file_size)  
            {
                send_pack(FILE_FINI_RP,m_my_infor.username,"server",file_name);
                printf("接收完成\n");
                print_main_menu();
                return NULL;  
            }
        }
 
        pthread_mutex_unlock(&mutex_recv_file);
        usleep(10);
    }   
}
 

//报错函数--------------------------------------------------------------------------------------------------------------------------- 
void my_err(const char * err_string,int line)
{
    fprintf(stderr, "line:%d  ", line);
    perror(err_string);
    exit(1); 
}

//关闭套接字
void sig_close(int i)
{
    //关闭客户端的描述符
    close(sockfd);
    exit(0);
}
 
//得到文件大小
//在这之前必须先创建好文件
int get_file_size(char *file_name)
{
    int fd;
    int len;
    if((fd = open(file_name,O_RDONLY)) == -1)
    {
        my_err("open",__LINE__);
        return 0;
    }
    len = lseek(fd, 0, SEEK_END);
    close(fd);
    return len;
}
 
//从文件数组中删除文件信息
void file_infor_delete(int id)
{
    pthread_mutex_lock(&mutex_local_user); 
    for(int j = id ;j <=m_recv_num_file_mes ;j++)
    {
        m_pack_recv_file_mes[j]  = m_pack_recv_file_mes[j+1];
    }
    m_recv_num_file_mes--;
    pthread_mutex_unlock(&mutex_local_user); 
}
 
 
//判断是否有重复的群聊
int judge_same_group(char *group_name)
{
    int i;
    for(i=1;i<=m_my_infor.group_num;i++)
    {
        if(strcmp(m_my_infor.group[i].group_name,group_name) == 0)
            return 1;
    }
    return 0;
}
 
//判断是否有重复的好友
int judge_same_friend(char add_friend[]){
    printf("friend's name %s\n",add_friend);
    for(int i = 0;i <= m_my_infor.friends_num;i++){
        printf("////////////%s\n",m_my_infor.friends[i].name);
        if(strcmp(m_my_infor.friends[i].name,add_friend) == 0){
            printf("找到好友： %s  i = %d\n",m_my_infor.friends[i].name,i);
            return i;
        }
    }
    return -1;
}
 
//为避免输入时出现的意外错误，进行字符串解析
//非数字字符，返回-1
int get_choice(char *choice_t)
{
    int choice =0;
    for(int i=0;i<strlen(choice_t) ;i++)
        if(choice_t[i]<'0' || choice_t[i]>'9')
            return -1;

    //数字字符转化为数字
    //--------------------------------------------------?????????
    for(int i=0;i<strlen(choice_t);i++)
    {
        int t=1;
        for(int j=1;j<strlen(choice_t)-i;j++)
        {
            t *=10;
        }
        choice += t*(int)(choice_t[i] - 48);
    }
    return choice;
}
 
//根据参数信息发送包
void send_pack(int type,char *send_name,char *recv_name,char *mes)
{
    if(type == 0){
        return;
    }
    PACK pack_send_pack;
    time_t timep;
    pack_send_pack.type = type;
    strcpy(pack_send_pack.data.send_name,send_name);
    strcpy(pack_send_pack.data.recv_name,recv_name);
    strcpy(pack_send_pack.data.mes,mes); 
    time(&timep);
    if(send(sockfd,&pack_send_pack,sizeof(PACK),0) < 0){
        my_err("send",__LINE__);
    }
}
 
 
//根据参数信息发送包,用于聊天信息发送
void send_pack_memcpy(int type,char *send_name,char *recv_name,char *mes)
{
    PACK pack_send_pack;
    time_t timep;
    pack_send_pack.type = type;
    strcpy(pack_send_pack.data.send_name,send_name);
    strcpy(pack_send_pack.data.recv_name,recv_name);
    memcpy(pack_send_pack.data.mes,mes,MAX_CHAR*2); 
    time(&timep);
    if(send(sockfd,&pack_send_pack,sizeof(PACK),0) < 0){
        my_err("send",__LINE__);
    }
}