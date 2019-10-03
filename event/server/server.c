#include"server.h"
//添加好友同意
PACK recv_t_add_agree;


int main()
{
    int n;
    
    pthread_t pid;
    /* PACK recv_t; */
    PACK *recv_pack;
    int err,socketfd,fd_num;
    struct sockaddr_in fromaddr;
    socklen_t len = sizeof(fromaddr);
    struct epoll_event ev, events[LISTENMAX];
     
    //mysql开关，链接mysql
    conect_mysql_init();
 
    //读取用户信息
    read_users_infor();

    //CTRL+C退出
    signal(SIGINT,signal_close);
    pthread_mutex_init(&mutex, NULL);  
 
 
    printf("服务器开始启动..\n");
    
    //开启发送离线包线程
    init_server_pthread();
    
    epollfd = epoll_create(EPOLL_MAX);//生成epoll句柄
    
    listenfd = socket(AF_INET,SOCK_STREAM,0);//启动socket
    if(listenfd == -1){
        perror("创建socket失败");
        printf("服务器启动失败\n");
        exit(-1);
    }
 
    err = 1;
 
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&err,sizeof(int)); 
 
    ev.data.fd = listenfd;//设置与要处理事件相关的文件描述符
    ev.events = EPOLLIN;//设置要处理的事件类型
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);//注册epoll事件
 
    //准备网络通信地址
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(bind(listenfd,(SA*)&addr,sizeof(addr))==-1){//绑定服务器
        perror("绑定失败");
        printf("服务器启动失败\n");
        exit(-1);
    }
    printf("成功绑定\n");
 
    //设置监听
    if(listen(listenfd,10)==-1){
        perror("设置监听失败");
        printf("服务器启动失败\n");
        exit(-1);
    }
    printf("设置监听成功\n");
    printf("初始化服务器成功\n");
    printf("服务器开始服务\n");
    
    
    while(1)
    {
        //等待事件产生
        fd_num = epoll_wait(epollfd, events, EPOLL_MAX, 1000);
        
        for(int i=0; i<fd_num; i++)
        {
            if(events[i].data.fd == listenfd)
            {   
                //接收套接字
                socketfd = accept(listenfd,(SA*)&fromaddr,&len);
                printf("%d 连接成功\n",socketfd);
                ev.data.fd = socketfd;
                ev.events = EPOLLIN;//设置监听事件可写
                
                //新增套接字
                epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &ev);
            }
            else if(events[i].events & EPOLLIN)//fd can read
            {
 
                n = recv(events[i].data.fd,&recv_t_add_agree,sizeof(PACK),0);//读取数据
                
                recv_t_add_agree.data.send_fd = events[i].data.fd;
                
                /* if(recv_t_add_agree.type == 0){
                    continue;
                } */

                //输出收到的包信息
                printf("\n\n\n ******接收包信息******\n");
                printf(" type     : %d\n", recv_t_add_agree.type);
                printf(" send_name: %s\n", recv_t_add_agree.data.send_name);
                printf(" recv_name: %s\n", recv_t_add_agree.data.recv_name);
                printf(" mes      : %s\n", recv_t_add_agree.data.mes);
                printf(" send_fd   : %d\n",recv_t_add_agree.data.send_fd);
                printf(" recv_fd   : %d\n",recv_t_add_agree.data.recv_fd);
                printf(" send_pack_num:%d\n", m_send_num);
                printf(" group_chat : %s\n",recv_t_add_agree.data.group_chat);
                printf("*******************\n\n");
 
                if(n < 0)//recv错误
                {     
                    close(events[i].data.fd);
                    perror("recv");
                    continue;
                }
                else if(n == 0)
                {
                    //客户端下线后，把客户端状态设置为下线
                    for(int j=1;j<=user_num;j++)
                    {
                        if(events[i].data.fd == user_infor[j].socket_id)
                        {
                            printf("%s 下线了!\n",user_infor[j].username);
                            user_infor[j].statu = DOWNLINE;
                            break;
                        }
                    }   
                    ev.data.fd = events[i].data.fd;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);//删除套接字
                    close(events[i].data.fd);
                    print_infor_user();
                    continue;   
                }
                
                
                //pthread_mutex_lock(&mutex); 
                //print_send_pack();
                //pthread_mutex_unlock(&mutex);                 
                //printf("m_file_num1:%d\n", m_file_num);
                //print_infor_file();
                // printf("m_file_num2:%d\n", m_file_num);
                //printf("hahah ****\n"); 
 
                int type =  0;
                type = recv_t_add_agree.type;
                recv_pack = (PACK*)malloc(sizeof(PACK));
                memcpy(recv_pack, &recv_t_add_agree, sizeof(PACK));
                
 
                //开启线程处理接受到的包
                if(pthread_create(&pid,NULL,deal,(void *)recv_pack) != 0)
                    my_err("pthread_create",__LINE__);
                if(type == FILE_SEND)
                {
                    printf("wati...\n");
                    usleep(100000);
                }
            }
 
        }
    }
    return 0;
}




//启动 
//处理包的函数，每接受到一个包，开新线程，根据类型进行处理
void *deal(void *recv_pack_t)
{
    /* int i; */
    PACK *recv_pack = (PACK *)recv_pack_t;
    printf("\n\n\ndeal function = %d\n", recv_pack->type);
  
    switch(recv_pack->type)
    {
        //登录
        case LOGIN:
            login(recv_pack);
            printf("LOGIN success!\n");
            break;
        //注册
        case REGISTER:
            registe(recv_pack);
            break;
        //查看好友
        case FRIEND_SEE:
            //printf("\n================\n");
            send_statu(recv_pack);
            //printf("\n--------------------\n");
            break;
        //添加好友
        case FRIEND_ADD:
            friend_add(recv_pack);
            break;
        //同意
        case AGREE:
            friend_add_agree(recv_pack);
            break;
        //删除好友
        case FRIEND_DEL:
            friend_del(recv_pack);
            break;
        //创建群组
        case GROUP_CREATE:
            group_create(recv_pack);
            break;
        //加入群组
        case GROUP_JOIN:
            group_join(recv_pack);
            break;
        //退出群组
        case GROUP_QIUT:
            group_qiut(recv_pack);
            break;
        //解散群组
        case GROUP_DEL:
            group_del(recv_pack);
            break;
        //私聊
        case CHAT_ONE:
            send_mes_to_one(recv_pack);
            break;
        //群聊
        case CHAT_MANY:
            send_mes_to_group(recv_pack);
            break;
        //发送文件
        case FILE_SEND_BEGIN:
            file_recv_begin(recv_pack);
            break;
        case FILE_SEND:
            file_recv(recv_pack);
            break;
        case FILE_SEND_BEGIN_RP:
            file_send_begin(recv_pack);
            break;
        case FILE_FINI_RP:
            file_send_finish(recv_pack);
            break;
        //消息记录
        case GROUP_RECORD:
        case USERS_RECORD:
            send_record(recv_pack);
            break;
        case EXIT:
            break;       
    }
}
 
 
 
//包发送函数，不断遍历存储包的数组，如果要发送的客户端在线则发送，否则一直等到其上线，
//实现离线功能
void *serv_send_thread(void *arg)
{
    int user_statu = DOWNLINE;
    /* int id_stop; */
    int i,recv_fd_t,recv_fd_online;
    while(1)
    {
        pthread_mutex_lock(&mutex);//线程锁，保护存包数组 
        // printf("serv_send_thread:%d\n", m_send_num);
        
        /* printf("===============================================================\n"); */
        //用户状态，下线
        user_statu = DOWNLINE;

        //m_send_num发送包数目
        for(i = m_send_num-1;i>=0;i--)
        {
            INFO_USER *p_user = user_infor;
            for(int j =1; j <= user_num ;j++)
            {
                //检测用户是否在线
                if(strcmp(m_pack_send[i].data.recv_name,p_user->username) == 0)
                {
                    user_statu = p_user->statu;
                    if(user_statu == ONLINE)
                        //接收方套接字
                        recv_fd_online = p_user->socket_id;
                    break;
                }
                p_user = p_user->next;
            }

            /* printf("===============================================================\n"); */

            //上线，则发送包
            if(user_statu == ONLINE || m_pack_send[i].type == LOGIN || m_pack_send[i].type == REGISTER)
            {
                
                if(user_statu == ONLINE)
                    recv_fd_t = recv_fd_online;
                else
                    recv_fd_t = m_pack_send[i].data.recv_fd;
                
                /* printf("recv_fd_t = %d\n",recv_fd_t);

                printf("\n\n*****发送包****\n");
                printf(" type    :%d\n",m_pack_send[i].type);
                printf(" from    : %s\n",m_pack_send[i].data.send_name);
                printf(" to      : %s\n",m_pack_send[i].data.recv_name);
                printf(" mes     : %s\n",m_pack_send[i].data.mes);
                printf(" recv_fd : %d\n",m_pack_send[i].data.recv_fd);
                printf(" group_chat:%s\n",m_pack_send[i].data.group_chat);
                m_send_num-- ;
                printf(" pack left Now is:%d\n\n", m_send_num); */
 
                if(send(recv_fd_t,&m_pack_send[i],sizeof(PACK),0) < 0){
                    my_err("send",__LINE__);
                }

                printf("\n\n*****发送包****\n");
                printf(" type    :%d\n",m_pack_send[i].type);
                printf(" from    : %s\n",m_pack_send[i].data.send_name);
                printf(" to      : %s\n",m_pack_send[i].data.recv_name);
                printf(" mes     : %s\n",m_pack_send[i].data.mes);
                printf(" recv_fd : %d\n",m_pack_send[i].data.recv_fd);
                printf(" group_chat:%s\n",m_pack_send[i].data.group_chat);
                printf(" mes_int   :%d\n", m_pack_send[i].data.mes_int);
                printf(" friends num %d\n",m_pack_send[i].user.friends_num);
                m_send_num-- ;
                printf(" pack left Now is:%d\n\n", m_send_num);
                
                for(int j = i;j <=m_send_num && m_send_num;j++)
                {
                    m_pack_send[j] = m_pack_send[j+1];
                }
                break;
            }
        }
        pthread_mutex_unlock(&mutex);
        usleep(1);  
    }
}
 
 
//不断检测文件状态，当文件传输失败，发送提醒给客户端
void *pthread_check_file(void *arg)
{
    while(1)
    {
       
        pthread_mutex_lock(&mutex_check_file);  
        for(int i=1 ;i<=m_file_num ;i++)
        {
            INFO_USER *p;
            //用户突然下线，发送提醒给客户端
            if(m_infor_file[i].file_size <= m_infor_file[i].file_size_now&&(m_infor_file[i].flag == FILE_STATU_RECV_ING ||m_infor_file[i].flag == FILE_STATU_RECV_STOP))
            {
                
                char mes_t[MAX_CHAR];
                printf("**********文件状态******** \n");
                
                p = find_userinfor(m_infor_file[i].file_recv_name);

                PACK *pthread_check_file_t = (PACK*)malloc(sizeof(PACK));
                memset(pthread_check_file_t, 0,sizeof(PACK));
                pthread_check_file_t->type = FILE_RECV_BEGIN;
                strcpy(pthread_check_file_t->data.send_name ,m_infor_file[i].file_send_name);
                strcpy(pthread_check_file_t->data.recv_name,m_infor_file[i].file_recv_name);
                
                int len = m_infor_file[i].file_size;
 
                memset(mes_t,0,sizeof(mes_t));
                int digit = 0;
                while(len != 0)
                {   
                    mes_t[digit++] = len%10;
                    len /= 10;
                }
                mes_t[digit]  = -1;
                for(int j=0 ;j< SIZE_PASS_NAME ;j++)
                {
                    mes_t[NUM_MAX_DIGIT+j] = m_infor_file[i].file_name[j];
                }  
 
 
                memcpy(pthread_check_file_t->data.mes,mes_t,sizeof(mes_t));  
                send_pack(pthread_check_file_t);
                free(pthread_check_file_t);
                m_infor_file[i].flag = FILE_STATU_SEND_ING;
    
                break;
            }
 
            //文件上传完毕，发送下载提醒给客户端
            if(m_infor_file[i].file_size > m_infor_file[i].file_size_now )
            {
                p = find_userinfor(m_infor_file[i].file_send_name);
                if(p->statu == DOWNLINE && m_infor_file[i].flag == FILE_STATU_RECV_ING)
                {
                    printf(" file_name %s d\n", m_infor_file[i].file_name);
                    
                    char mes[MAX_CHAR];
                    memset(mes,0,sizeof(mes));
                    int num = m_infor_file[i].file_size_now;
                    
                    printf(" file_size :%d\n",num );
 
                    int digit = 0;
                    while(num != 0)
                    {   
                        mes[digit++] = num%10;
                        num /= 10;
                    }
                    mes[digit] = -1;
                    for(int i=0;i<10;i++)
                        printf("%d\n\n",mes[i]);
 
                    printf(" file_name %s \n", m_infor_file[i].file_name);
                   
 
                    PACK pthread_check_file_t;
                    memset(&pthread_check_file_t, 0,sizeof(PACK));
                    pthread_check_file_t.type = FILE_SEND_STOP_RP;
                    strcpy(pthread_check_file_t.data.send_name ,m_infor_file[i].file_name);
                    strcpy(pthread_check_file_t.data.recv_name,m_infor_file[i].file_send_name);
                    memcpy(pthread_check_file_t.data.mes,mes,sizeof(mes));
                    
                    send_pack(&pthread_check_file_t);
                    m_infor_file[i].flag = FILE_STATU_RECV_STOP;
                    break;
                }
            }
            //删除服务端缓存文件，并从文件数组中清除该文件信息
            if(m_infor_file[i].flag == FILE_STATU_SEND_FINI)
            {
                unlink(m_infor_file[i].file_name);
                m_file_num --;
                for(int j = i;j<=(m_file_num+1)&&m_file_num;j++)
                {
                    m_infor_file[j] = m_infor_file[j+1];
                } 
 
            }     
        }
        pthread_mutex_unlock(&mutex_check_file); 
        usleep(10);
    }    
}
 
//开启线程
void init_server_pthread()
{
    printf("\ninit_server_pthread\n");
    pthread_t pid_send;
    /* pthread_t pid_file_check; */
    
    pthread_create(&pid_send,NULL,serv_send_thread,NULL);
    //pthread_create(&pid_file_check,NULL,pthread_check_file,NULL);
} 
 



//功能函数========================================================================================================================
//跟据收到包的内容，找到登录者的信息
void login(PACK *recv_pack)
{
    int id=0;
    char login_flag[10];
    
    INFO_USER *p;
    
    p = find_userinfor(recv_pack->data.send_name);
    
    printf("======%s %d\n",p->username,p->statu);

    if(p == NULL){
        //没有注册
        //printf("------------------------------1");
        login_flag[0] = '2';
    }
    else if (p->statu == ONLINE)
    {
        //printf("------------------------------2");
        //已登录
        login_flag[0] = '3';
 
    }
    else if(strcmp(p->password,recv_pack->data.mes) == 0){
        //printf("------------------------------3");
        //登录成功
        login_flag[0] = '1';
        //change user infor
        
        p->socket_id = recv_pack->data.send_fd;
        p->statu = ONLINE;
        printf("\n\n********登录**********\n");
        printf(" %s 登陆成功!\n", p->username);
        printf(" statu:    %d\n", p->statu); 
        printf(" socket_id:%d\n\n",p->socket_id);
    }   
      
    else
        //密码不匹配
        login_flag[0] = '0';
    login_flag[1] = '\0';
    /*printf("%s\n", recv_pack->data.send_name);
    printf("%s\n", recv_pack->data.mes);
    printf("%d\n", m_user_num); */    
    
    //包信息赋值
    strcpy(recv_pack->data.recv_name,recv_pack->data.send_name);
    strcpy(recv_pack->data.send_name,"server");
    strcpy(recv_pack->data.mes,login_flag);
    recv_pack->data.recv_fd = recv_pack->data.send_fd;
    recv_pack->data.send_fd = listenfd;
    
    //发送包
    send_pack(recv_pack);
    //考虑到中间的数据传输耗时，停顿若干时间，再修改状态
    usleep(10);
    if(login_flag[0] =='1')
        user_infor[id].statu = ONLINE;
    free(recv_pack);
    //printf("sdgvfsrevgertgvrt\n");
}
 
 
 
 
//注册
void registe(PACK *recv_pack)
{
    char registe_flag[10];
 
    if(judge_usename_same(recv_pack->data.send_name)){//the password is not crrect
         
        registe_flag[0] = '1';
        //add user infor
        
        //用户数+1
        user_num++;
        strcpy(user_infor[user_num].username,recv_pack->data.send_name);
        
        //printf("\n---------------------------%s\n",user_infor[user_num].password);
        strcpy(user_infor[user_num].password,recv_pack->data.mes);

        
        printf("\n\n******注册***** \n");
        printf(" regist success!\n");
        printf(" username:%s   passward:%s\n", user_infor[user_num].username,user_infor[user_num].password);
        /* printf("\033[;44m*\033[0m password:%s\n", user_infor[user_num].password); */
        printf("m user_num:%d\n\n", user_num);
        
                                                      
        user_infor[user_num].statu = DOWNLINE;
    }
    else 
        registe_flag[0] = '0';
    registe_flag[1] = '\0';
    
    //包信息赋值
    strcpy(recv_pack->data.recv_name,recv_pack->data.send_name);
    strcpy(recv_pack->data.send_name,"server");
    //注册成功
    strcpy(recv_pack->data.mes,registe_flag);
    
    recv_pack->data.recv_fd = recv_pack->data.send_fd;
    recv_pack->data.send_fd = listenfd;
    
    //发送包
    send_pack(recv_pack);
    free(recv_pack);
}
 
 
//把该客户端的朋友信息返回给客户端
//查看好友
void send_statu(PACK *recv_pack)
{
    //printf("----------------send begin!\n");
    char recv_name[MAX_CHAR];
    char name_t[MAX_CHAR];
    char send_statu_mes[MAX_CHAR*2];
    int id;
    int count = 0;
    INFO_USER *p,*q;

    memset(send_statu_mes,0,sizeof(send_statu_mes));
    
    //寻找用户id
    p = find_userinfor(recv_pack->data.send_name);
    //send_statu_mes[count++] = p->friends_num;
    
    strcpy(recv_name,recv_pack->data.send_name);
    

    
    //好友信息
    //好友数目
    //recv_pack->user.friends_num = p->friends_num;
    //strcpy(recv_pack->user.username,recv_pack->data.recv_name);
    //printf("===========%d\n",recv_pack->user.friends_num);
    
    for(int i = 0;i < 2;i++){
        printf("------------%s-----%d----------\n",p->friends[i],p->friends_num);
    }
    
    for(int i = 0;i < p->friends_num;i++){
        //好友名称
        strcpy(recv_pack->data.mes_2[i],p->friends[i]);
        if((q = find_userinfor(p->friends[i]))== NULL){
            //printf("\ni = %d\n",i);
            break;
        }
        //状态
        recv_pack->data.mes_2_st[i] = q->statu;
        //printf("-------------user %s  friend %d %s\n",recv_pack->data.send_name,i,recv_pack->data.mes_2[i]);
    }
    
    recv_pack->data.mes_int = p->friends_num;

    printf("aaaaaaaaaaaaaaaaaaaaa%d\t,%d\n",recv_pack->data.mes_int,p->friends_num);
    for(int i = 0;i < recv_pack->user.friends_num;i++){
        printf("\n%d    %s  \n",recv_pack->user.friends[i].statu,recv_pack->user.friends[i].name);
    }
    
    
    
    //recv_pack->data.mes_int = 99;
    strcpy(recv_pack->data.send_name,"server");
    strcpy(recv_pack->data.recv_name,recv_name);
    recv_pack->data.recv_fd = recv_pack->data.send_fd;
    recv_pack->data.send_fd = listenfd;
    send_pack(recv_pack);
    
    //printf("----------------2 send begin!\n");
    /* //群组信息
    //群组数目
    recv_pack->user.group_num = p->group_num;
    for(int i = 0;i < p->group_num;i++){
        recv_pack->group[i] = p->group[i]; 
    }

    printf("----------------3 send begin!\n");
    strcpy(recv_pack->data.recv_name,recv_name);
    strcpy(recv_pack->data.send_name,"server");
        recv_pack->data.recv_fd = recv_pack->data.send_fd;
        recv_pack->data.send_fd = listenfd;
        recv_pack->data.type_2 = 1;
        
    send_pack(recv_pack); */


/*     printf("----------------send begin!   %s   %d\n",p->username,p->friends_num);
    for(int i = 0;i < p->friends_num;i++){
        recv_pack->data.mes_int = p->friends_num;
        strcpy(recv_pack->data.mes,p->friends[i]);
        char c[5];
        c[0] = p->statu - '0';
        c[1] = '\0';
        strcpy(recv_pack->data.group_chat,c);
        
        strcpy(recv_pack->data.recv_name,recv_name);
        
        strcpy(recv_pack->data.send_name,"server");
        recv_pack->data.recv_fd = recv_pack->data.send_fd;
        recv_pack->data.send_fd = listenfd;
        recv_pack->data.type_2 = 1;
        
        send_pack(recv_pack);
        printf("----------------friends send success!\n");
    }

    char c[5];
    for(int i =0;i < p->group_num;i++){
        
        
        
        strcpy(recv_pack->group[i].group_name,p->group[i].group_name);
        strcpy(recv_pack->group[i].group_member_num,p->group[i].group_member_num);
        recv_pack->group[i].kind = p->group[i].kind;
        for(int j = 0;j < p->group[i].group_member_num;i++)
            strcpy(recv_pack->group[i].group_member_name[j],p->group[i].group_member_name[j]);
        
        
        
        recv_pack->data.mes_int = p->group_num;
        recv_pack->group[i] = p->group[i];

        strcpy(recv_pack->data.recv_name,recv_name);
        strcpy(recv_pack->data.send_name,"server");
        recv_pack->data.recv_fd = recv_pack->data.send_fd;
        recv_pack->data.send_fd = listenfd;
        recv_pack->data.type_2 = 2;

        send_pack(recv_pack);
        printf("----------------groups send success!\n");
    }
 */





/* 
    //字符串处理
    for(int i=1 ;i <= p->friends_num ;i++)
    {
        strcpy(name_t,p->friends[i]);
        for(int j=1 ;j <= user_num ;j++)
        {
            if(strcmp(name_t,p->username) == 0)
            {
                memset(str,0,sizeof(str));
                //写入用户和所处状态
                if(user_infor[j].statu == ONLINE)
                    sprintf(str,"%d %s\0",ONLINE,p->username);
                else
                    sprintf(str,"%d %s\0",DOWNLINE,p->username);
                
                printf("str = %s\n",str);
                
                for(int k = 0 ;k < SIZE_PASS_NAME;k++)
                {
                    send_statu_mes[k+count] = str[k];
                }
                count += SIZE_PASS_NAME;
            }
        }
    } */
 


/*     send_statu_mes[count++] = p->group_num;
    
    for(int i = 1;i <=  p->group_num;i++)
    {
        memset(str,0,sizeof(str));
        strcpy(name_t,user_infor[id].group[i].group_name);
        sprintf(str,"%s\0",name_t);
        for(int k = 0 ;k< SIZE_PASS_NAME;k++)
        {
            send_statu_mes[k+count] = str[k];
        }
        count += SIZE_PASS_NAME;
    } */
    
    //包信息赋值
    /* strcpy(recv_pack->data.recv_name,recv_pack->data.send_name);
    strcpy(recv_pack->data.send_name,"server");
    memcpy(recv_pack->data.mes,send_statu_mes,MAX_CHAR*2);
    recv_pack->data.recv_fd = recv_pack->data.send_fd;
    recv_pack->data.send_fd = listenfd;
 
    send_pack(recv_pack); */
    free(recv_pack);
}
 
//跟据收到包的内容，添加好友
//同意添加
void friend_add_agree(PACK *recv_pack)
{
    int id_own,id_friend;
    INFO_USER *p_own,*p_friend;
    //寻早用户id
    
    p_own = find_userinfor(recv_pack->data.send_name);
    
    strcpy(p_own->friends[++(p_own->friends_num)],recv_pack->data.mes);
    
    p_friend = find_userinfor(recv_pack->data.mes);
    
    strcpy(p_friend->friends[++(p_friend->friends_num)],recv_pack->data.send_name);
    
    free(recv_pack);
}

//请求添加
void friend_add(PACK *recv_pack){
    
    char *mes = recv_t_add_agree.data.send_name;
    strcpy(recv_pack->data.recv_name,recv_pack->data.mes);
    strcpy(recv_pack->data.send_name,"server");
    memcpy(recv_pack->data.mes,mes,MAX_CHAR*2);
    recv_pack->type = AGREE;
    recv_pack->data.recv_fd = recv_pack->data.send_fd;
    recv_pack->data.send_fd = listenfd;

    send_pack(recv_pack);
    free(recv_pack);
}
 
//删除好友
void friend_del(PACK *recv_pack)
{
    int id_own,id_own_1;
    INFO_USER *p_own,*p_own_1;
    INFO_USER *p_friend,*p_friend_1;
    
    p_own = find_userinfor(recv_pack->data.send_name);
    
    del_friend_infor(p_own,recv_pack->data.mes); 
    
    p_friend = find_userinfor(recv_pack->data.mes);
    
    printf("friend_name == %s\n",p_friend->username);
    
    del_friend_infor(p_friend,recv_pack->data.send_name); 
    
    free(recv_pack);
}
 
//发送信息，并存储
void send_mes_to_one(PACK *recv_pack)
{
    //存储到数据库
    mysql_save_message(recv_pack,USERS_RECORD);
    //printf("--------------------sssssssssssssssssssss-----------------------------\n");
    send_pack(recv_pack);
    free(recv_pack);
}
 
//创建群
void group_create(PACK *recv_pack)
{
    INFO_GROUP *p_group;

    //判断是否已经建立
    //若已经建立怎发送消息
    p_group = find_groupinfor(recv_pack->data.mes);
    if(p_group == NULL){
        strcpy(recv_pack->data.recv_name,recv_pack->data.send_name);
        strcpy(recv_pack->data.send_name,"server");
        recv_pack->data.mes[0] = 1;
        send_pack(recv_pack);
        free(recv_pack);
        return ;
    }

    p_group = p_group->prev;

    INFO_GROUP *new_group = (INFO_GROUP *)malloc(sizeof(INFO_GROUP));
    INFO_USER *p_user;

    new_group->member_num = 0;
    //新建群信息
    //群名
    strcpy(new_group->group_name,recv_pack->data.mes);
    //群主加入群
    strcpy(new_group->member_name[new_group->member_num++],recv_pack->data.send_name);
    //找到该用户
    p_user = find_userinfor(recv_pack->data.send_name);
    //群信息赋值给该用户
    strcpy(p_user->group[p_user->group_num].group_name,recv_pack->data.mes);
    //设置为群主
    p_user->group[p_user->group_num].kind = 1;
    //群数目+1
    List_InsertAfter(p_group,new_group);
    group_num++;
 
    printf("\n\ncreat group : %s  successfully! \n\n", recv_pack->data.mes);
    strcpy(recv_pack->data.recv_name,recv_pack->data.send_name);
    strcpy(recv_pack->data.send_name,"server");
 
    recv_pack->data.mes[0] = 2;
    send_pack(recv_pack);
    free(recv_pack);
}
 
 
//加群
void group_join(PACK *recv_pack)
{
    INFO_GROUP *p_group = find_groupinfor(recv_pack->data.mes);

    //找到该群，并加入
        //找到群
        if(p_group != NULL){
            
            //用户加入群
            strcpy(p_group->member_name[p_group->member_num++],recv_pack->data.send_name);
            
            //寻找用户id，将群组名称加入到用户信息中
            INFO_USER *p_user=find_userinfor(recv_pack->data.send_name);
            strcpy(p_user->group[p_user->group_num++].group_name,recv_pack->data.mes);
            p_user->group[p_user->group_num].kind = 0;

            //给包赋值
            strcpy(recv_pack->data.recv_name,recv_pack->data.send_name);
            strcpy(recv_pack->data.send_name,"server");

            recv_pack->data.mes[0] = 2; 
            
            printf("\n\n %s join group : %s  successfully! \n\n",recv_pack->data.recv_name, recv_pack->data.mes);
            print_infor_group();
            print_infor_user();
            
            //发包
            send_pack(recv_pack);
            free(recv_pack);
            return ;
        }
    //未找到该群   
    strcpy(recv_pack->data.recv_name,recv_pack->data.send_name);
    strcpy(recv_pack->data.send_name,"server");
 
    recv_pack->data.mes[0] = 1;
    send_pack(recv_pack);
    free(recv_pack);
}
 
 
//退群
void group_qiut(PACK *recv_pack)
{
    del_group_from_user(recv_pack->data.send_name,recv_pack->data.mes);
    
    INFO_GROUP *p_group = find_groupinfor(recv_pack->data.mes);

            for(int j = 0;j < p_group->member_num;j++)
            {
                if(strcmp(recv_pack->data.send_name,p_group->member_name[j]) == 0)
                {
                    for(int k = j;k < p_group->member_num;k++)
                    {
                        strcpy(p_group->member_name[k],p_group->member_name[k+1]);
                    }
                    p_group->member_num--;
                    print_infor_group();
                }
            }
}
 

//查看群成员
void check_group(PACK *recv_pack){

}



//删除一个群组，并从其群成员的信息中删除该组的信息
void group_del_one(int id)
{
    for(int i=1;i <= group_infor[id].member_num;i++)
    {
        del_group_from_user(group_infor[id].member_name[i],group_infor[id].group_name);
    }
 
    for(int i = id ;i < group_num;i++)
    {
        group_infor[i] = group_infor[i+1];
    }
    group_num--;
}
 
//解散群（首先会判断是否为群主）
void group_del(PACK *recv_pack)
{
    INFO_GROUP *p_group = find_groupinfor(recv_pack->data.mes);
    INFO_USER *p_user = find_userinfor(recv_pack->data.send_name);
    int i = 0;
    for(i = 0;i < p_user->group_num;i++){
        if(strcmp(p_user->group[i].group_name,recv_pack->data.mes) == 0){
            printf("已经在用户中找到群组\n");
            break;
        }
    }
    if(i == group_num){
        printf("没有在用户中找到这个群组\n");
        return ;
    }
    //判断是否为群主
    if(p_user->group[i].kind == 1){
        group_del_one(i);
        recv_pack->data.mes[0] = 2;
        printf("\n\n delete group : %s  successfully! \n\n",recv_pack->data.mes);
    }
    //非群主
    else
        recv_pack->data.mes[0] = 1;

    strcpy(recv_pack->data.recv_name,recv_pack->data.send_name);
    strcpy(recv_pack->data.send_name,"server");
    send_pack(recv_pack);
    free(recv_pack);
}
 

//群聊
//跟据收到包的内容，利用循环给群成员每一个人都发一遍，并存储到mysql数据库
void send_mes_to_group(PACK *recv_pack)
{
 
    INFO_GROUP *p_group;
    
    //寻找群
    p_group = find_groupinfor(recv_pack->data.recv_name);
    
    //消息长度
    int len_mes = strlen(recv_pack->data.mes);
    char send_name[SIZE_PASS_NAME];
 
    //保存到群聊天记录
    mysql_save_message(recv_pack,GROUP_RECORD);
    
    for(int i = len_mes;i >= 0;i--){
        recv_pack->data.mes[i+SIZE_PASS_NAME] = recv_pack->data.mes[i];
    }
    
    //消息来源
    strcpy(recv_pack->data.group_chat,recv_pack->data.send_name);

    /*for(int i=0;i<SIZE_PASS_NAME;i++){
        recv_pack->data.mes[i] = recv_pack->data.send_name[i];
    }*/

    
    //将群组设置为发送方
    strcpy(recv_pack->data.send_name,recv_pack->data.recv_name);
    

    for(int i = 0; i < p_group->member_num;i++)
    {
        //群组中的人改成接收方
        strcpy(recv_pack->data.recv_name,p_group->member_name[i]);
        
        if(strcmp(recv_pack->data.group_chat,p_group->member_name[i]) != 0){
            //printf("\n-----11111----\n");
            /* mysql_save_message(recv_pack); */   
            send_pack(recv_pack);
        }
    }
    free(recv_pack);
}


//收发文件
//客户端请求发送文件
//判断是否已经有该文件信息
//有，返回该文件的大小
//否则，建立文件信息，并返回文件大小为0
void file_recv_begin(PACK *recv_pack)
{
    int flag = 0;
    int i;
    int file_size_now_t;
 
    pthread_mutex_lock(&mutex_check_file);  
    for(i=1 ;i<= m_file_num ;i++)
    {
         //文件存在
        if(strcmp(m_infor_file[i].file_name,recv_pack->data.mes+NUM_MAX_DIGIT) == 0)
        {
            file_size_now_t = m_infor_file[i].file_size_now;
            flag = 1;
            break;
        } 
    }
    //不存在，则建立文件信息
    if(!flag)
    {
        file_size_now_t = 0;
        strcpy(m_infor_file[++m_file_num].file_name,recv_pack->data.mes+NUM_MAX_DIGIT);
        strcpy(m_infor_file[m_file_num].file_send_name,recv_pack->data.send_name);
        strcpy(m_infor_file[m_file_num].file_recv_name,recv_pack->data.recv_name);
        
        //解析文件大小
        int t=0;
        for(int k=0;k<NUM_MAX_DIGIT;k++)
        {
            if(recv_pack->data.mes[k] == -1)
                break;
            int t1 = 1;
            for(int l=0 ;l<k;l++)
                t1*=10;
            t += (int)(recv_pack->data.mes[k])*t1;
        }
       //建立文件信息 
        m_infor_file[m_file_num].file_size = t;
        m_infor_file[m_file_num].file_size_now  = 0;
        m_infor_file[m_file_num].flag = FILE_STATU_RECV_ING;
    }
 
    pthread_mutex_unlock(&mutex_check_file);  
    
    recv_pack->type = FILE_SEND_BEGIN_RP;
    strcpy(recv_pack->data.recv_name,recv_pack->data.send_name);
    
    int digit = 0;
    while(file_size_now_t != 0)
    {   
        recv_pack->data.mes[digit++] = file_size_now_t%10;
        file_size_now_t /= 10;
    }
    recv_pack->data.mes[digit]  = -1;
    
    send_pack(recv_pack);
    free(recv_pack);
}
 
 
//就把文件内容写入到文件中
void file_recv(PACK *recv_pack)
{
//pthread_mutex_lock(&mutex_recv_file);
    int fd;
    char file_name[MAX_CHAR];
    char file_path_t[SIZE_PASS_NAME];
    
 
    int  len = 0;
    for(int i=0 ;i<NUM_MAX_DIGIT ;i++)
    {
        if(recv_pack->data.mes[i] == -1)  
            break;
        int t1 = 1;
        for(int l=0;l<i;l++)
            t1*=10;
        len += (int)recv_pack->data.mes[i]*t1;
 
    }
 
 
    
    strcpy(file_name,recv_pack->data.recv_name);
    if((fd = open(file_name,O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) < 0)
    {
        my_err("open",__LINE__);
        return ;
    }
 
    if(write(fd,recv_pack->data.mes + NUM_MAX_DIGIT,len) < 0)
        my_err("write",__LINE__);
    // 关闭文件 
    close(fd);
    
    int id  =  find_fileinfor(file_name);
    m_infor_file[id].file_size_now += len;
    free(recv_pack);
    //pthread_mutex_unlock(&mutex_recv_file);  
    //输出文件信息
    print_infor_file();
}
 
 
 
 
//根据文件的起始位置，开始发送，
//在发送过程中，不断监测接收端的状态
//若接收端下线，则向接收端发送提醒消息
void *file_send_send(void *file_send_begin_t,char *file_name_t)
{
 
    PTHREAD_PAR *file_send_begin = (PTHREAD_PAR *)file_send_begin_t;
    int fd;
    int length;
    int statu;
    INFO_USER *p_user;
    int sockfd;
    int file_size;
    int begin_location = file_send_begin->a;
    int sum = begin_location;
    char file_name[SIZE_PASS_NAME];
    char recv_name[SIZE_PASS_NAME];
    char mes[MAX_CHAR*2];
    
    printf("\n\n发送文件.........\n");
    printf("file_name_t = %s\n",file_name_t); 

    /* strcpy(file_name,file_name_t); */
    
    strcpy(file_name,file_send_begin->str1);
    strcpy(recv_name,file_send_begin->str2);
 
    printf("file_name = %s\n",file_name);
    if((fd = open(file_name,O_RDONLY)) == -1)
    {
        my_err("open",__LINE__);
        return NULL;
    }
    
    file_size=lseek(fd, 0, SEEK_END);
    //定位到开始位置
    lseek(fd ,begin_location ,SEEK_SET);
    printf("文件大小 %d\n", file_size); 
    printf("开始位置%d\n", begin_location);
    bzero(mes, MAX_CHAR*2); 
 
    // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止 
    while((length = read(fd ,mes+NUM_MAX_DIGIT ,(MAX_CHAR*2 - NUM_MAX_DIGIT))) > 0) 
    {
        printf("send_::%d\n", sum);
        printf("file_size%d\n",file_size);
        if(sum >= file_size)  
            break;
 
        p_user = find_userinfor(recv_name);
        //当文件传输过程中，接收端下线，则向接收端发送提醒消息
        if(p_user->statu == DOWNLINE)
       {
            int  file_id   = find_fileinfor(file_name);
            int  file_size = m_infor_file[file_id].file_size;
            char mes_t[MAX_CHAR];
            PACK file_send_stop_t;
 
            memset(&file_send_stop_t, 0,sizeof(PACK));
            
            file_send_stop_t.type = FILE_RECV_STOP_RP;
            strcpy(file_send_stop_t.data.send_name ,m_infor_file[file_id].file_send_name);
            strcpy(file_send_stop_t.data.recv_name,m_infor_file[file_id].file_recv_name);
            
            memset(mes_t,0,sizeof(mes_t));
            //发送服务端已经发送的字节数
            int digit = 0;
            while(file_size != 0)
            {   
                mes_t[digit++] = file_size%10;
                file_size /= 10;
            }
            mes_t[digit]  = -1;
            for(int j=0 ;j< SIZE_PASS_NAME ;j++)
            {
                mes_t[NUM_MAX_DIGIT+j] = m_infor_file[file_id].file_name[j];
            }  
            memcpy(file_send_stop_t.data.mes,mes_t,sizeof(mes_t));  
            
            send_pack(&file_send_stop_t);
            free(file_send_begin);
            return NULL ;
        }
 
        sockfd = p_user->socket_id;
        
        //在发送数据的前面，为文件已发送的字节数 
        int t=length;
        int digit = 0;
        while(t != 0)
        {   
            mes[digit++] = t%10;
            t /= 10;
        }
        mes[digit]  = -1;
        
        send_pack_memcpy_server(FILE_RECV,file_name,recv_name,sockfd,mes);
        
        
        sum += length; 
        bzero(mes, MAX_CHAR*2); 
        //为防止发送顺序被打乱，发送时有一定时间间隔
        usleep(200000);
    } 
    // 关闭文件 
    close(fd);
    free(file_send_begin);
}
 
//客户端请求接收文件,
void file_send_begin(PACK *recv_pack)
{
    PTHREAD_PAR *file_send_begin_t;
    file_send_begin_t = (PTHREAD_PAR *)malloc(sizeof(PTHREAD_PAR));
    char recv_name[SIZE_PASS_NAME];
    char file_name[SIZE_PASS_NAME];
    int  begin_location=0;

    strcpy(recv_name,recv_pack->data.send_name);
    strcpy(file_name,recv_pack->data.recv_name);
   
    //解析出客户端已经接收文件字节数
    for(int i=0 ;i<NUM_MAX_DIGIT ;i++)
    {
        if(recv_pack->data.mes[i] == -1)  
            break;
        int t1 = 1;
        for(int l=0;l<i;l++)
            t1*=10;
        begin_location += (int)recv_pack->data.mes[i]*t1;
    }
    
    //寻找数组中文件的位置
    int file_id = find_fileinfor(file_name);
    

    /* printf("\n\n--------------------------------------------------aaaa = %s\n",file_name); */
    if(begin_location >= m_infor_file[file_id].file_size)
    {
        m_infor_file[file_id].flag  = FILE_STATU_SEND_FINI;
    }
 
    file_send_begin_t->a = begin_location;
    strcpy(file_send_begin_t->str1,file_name);
    strcpy(file_send_begin_t->str2,recv_name);
    
    //循环发送
    file_send_send((void *)file_send_begin_t,file_name);
    free(recv_pack);
}
 
//客户端接收完，会发送确认信息，
void file_send_finish(PACK *recv_pack)
{   
    int id = find_fileinfor(recv_pack->data.mes);
    m_infor_file[id].flag = FILE_STATU_SEND_FINI;
    free(recv_pack);
}




//数据库函数==============================================================================================================================
MYSQL           mysql;
MYSQL_RES       *res = NULL,*res_man = NULL;
MYSQL_ROW       row,row_t;
char            command[MAX_CHAR*4] ;
int             rc, fields,rc_man;
int             rows;
 
//链接mysql数据库
int conect_mysql_init()
{
    if (mysql_init(&mysql) == NULL) {               //初始化mysql变量
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        return -1;
    }
 
    if (NULL == mysql_real_connect(&mysql,        //链接mysql数据库
                MYSQL_LINK,                       //链接的当地名字
                MYSQL_USER,                       //用户名字
                MYSQL_PASSWARD,                   //用户密码
                MYSQL_NAME,                       //所要链接的数据库
                0,
                NULL,
                0)) {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
    printf("数据库%s连接成功! \n",MYSQL_NAME);
}
 
//关闭数据库
void mysql_close()
{
    mysql_free_result(res);
    mysql_close(&mysql);
} 

//从数据库中读取用户信息
int read_users_infor()
{

    //链表初始化
    List_Init(user_infor,INFO_USER);
    List_Init(group_infor,INFO_GROUP);

    //用户基本信息
    user_infor_find(user_infor);
    
    //用户好友信息
    user_friend_find(user_infor);

    //群组基础信息
    group_infor_find(group_infor);
    
    //群组成员读入
    group_member_find(user_infor);

    return 0;
}

//查找用户信息
int user_infor_find(INFO_USER *node){
    char command_user_name[MAX_CHAR*2];

    sprintf(command_user_name,"select *from zzy_chat_user_mes");

    int rc_user_name = mysql_real_query(&mysql, command_user_name, strlen(command_user_name));
    
    if (rc_user_name != 0){
        printf("mysql_real_query(): %s \t %d\n", mysql_error(&mysql),__LINE__);
        return 0;
    }

    res = mysql_store_result(&mysql);

    if (res == NULL) {
        printf("mysql_restore_result(): %s \t %d \n", mysql_error(&mysql),__LINE__);
        return 0;
    }

    int rows = mysql_num_rows(res);  
        printf("user_mse rows is: %d\n",rows);

    //读取用户账号密码
    user_num = 0;
    INFO_USER *p = user_infor,*q;
    while ((row = mysql_fetch_row(res))) {
        q = (INFO_USER*)malloc(sizeof(INFO_USER));
        strcpy(q->username,row[1]);
        strcpy(q->password,row[2]);
        q->statu = DOWNLINE;
        List_AddTail(p,q);
        user_num++;
    }

    //去除空的链表头
    p = user_infor;
    user_infor = user_infor->next;
    List_DelNode(p);
    
    printf("read user_mes success!\n");

    for(int i = 0;i < 4;i++){
        printf("");
    }

    return 1;
}

//好友查找
int user_friend_find(INFO_USER *node){
    INFO_USER *p = user_infor;
    char command[MAX_CHAR];
    int  num = 0;
    while(num != user_num){
        sprintf(command,"select *from zzy_chat_user_friend where user_1 = '%s' or user_2 = '%s'",p->username,p->username);

        int rc_user_friend = mysql_real_query(&mysql,command,strlen(command));

        if(rc_user_friend != 0){
             printf("mysql_real_query(): %s \t %d\n", mysql_error(&mysql),__LINE__);
            return 0;
        }

        res = mysql_store_result(&mysql);

        if(res == NULL){
            printf("mysql_restore_result(): %s \t %d \n", mysql_error(&mysql),__LINE__);
            return 0;
        }

        int rows = mysql_num_rows(res);
        printf("user_friend_find rows is %d \n",rows);

        int i = 0;
        //读入好友成员
        while(row = mysql_fetch_row(res)){
            if(strcmp(p->username,row[0]) == 0){
                strcpy(p->friends[i],row[1]);
                i++;
                continue;
            }
            strcpy(p->friends[i],row[0]);
            i++;
        }
        

        printf("user %s friend read success!\n",p->username);
        num++;
        p->friends_num = rows;
        p = p->next;
    }

    printf("user_friend_find success!\n");
    return 1;
}

//查找群组信息
int group_infor_find(INFO_GROUP *node){
    char command_group_name[MAX_CHAR*2];

    sprintf(command_group_name,"select *from zzy_chat_group_mes");

    int rc_group_name = mysql_real_query(&mysql, command_group_name, strlen(command_group_name));
    
    if (rc_group_name != 0){
        printf("mysql_real_query(): %s \t %d\n", mysql_error(&mysql),__LINE__);
        return 0;
    }

    res = mysql_store_result(&mysql);

    if (res == NULL) {
        printf("mysql_restore_result(): %s \t %d \n", mysql_error(&mysql),__LINE__);
        return 0;
    }

    int rows = mysql_num_rows(res);  
        printf("group rows is: %d\n",rows);

    //读取群组名称，成员个数信息
    group_num = 0;
    INFO_GROUP *p = group_infor,*q;
    char a[2];
    while((row = mysql_fetch_row(res))){
        q = (INFO_GROUP* )malloc(sizeof(INFO_GROUP));
        strcpy(q->group_name,row[0]);
        strcpy(a,row[1]);
        q->member_num = a[0] - '0';
        //printf("\n\n%s,%d\n",q->group_name,q->member_num);
        List_AddTail(p,q);
        group_num++;
        //printf("------%s------\n",q->group_name);
    }

    //去除空的链表头
    p = group_infor;
    group_infor = group_infor->next;
    List_DelNode(p);
    printf("read group_mes success!\n");

    return 1;
}

//查找群组内人员名单
int group_member_find(INFO_USER *node)
{   
    INFO_GROUP *p = group_infor;
    char command_group_members[MAX_CHAR*2];
    int num = 0;
    while(num != group_num){
        
        sprintf(command_group_members,"select *from zzy_chat_group_members where group_name = '%s' ",p->group_name);

        //printf("%d---------%s ------- %s\n",group_num,p->group_name,p->next->group_name);
        int rc_group_member = mysql_real_query(&mysql, command_group_members, strlen(command_group_members));
    
        if (rc_group_member != 0){
            printf("mysql_real_query(): %s \t %d\n", mysql_error(&mysql),__LINE__);
            return 0;
        }
    
        //获取查询结果
        MYSQL_RES *res_group_member = mysql_store_result(&mysql);

        int rows_group_member = mysql_num_rows(res_group_member);


        int rows = mysql_num_rows(res);  
            printf("group member rows is: %d\n",rows);

        int i = 0;
        char a[2],b[2];
        while(row = mysql_fetch_row(res_group_member)){
            strcpy(p->member_name[i],row[1]);
            //状态
            strcpy(a,row[2]);
            p->statue[i] = a[0] - '0';
            //群中职务
            strcpy(b,row[3]);
            p->kind[i] = b[0] - '0';
            i++;
        }

        if(i == p->member_num){
            printf("group %s infor read success!\n",p->group_name);
        }

        //printf("group_name %s i = %d,member = %d\n",p->group_name,i,p->member_num);

        num++;
        p = p->next;
    }

    printf("read group_mumbers success!\n");

    return 1;
}





//服务端关闭时把信息写入到文件中
int write_infor()
{
    
}














//mysql保存聊天记录数据
void mysql_save_message(PACK *recv_pack,int flag){
    if(flag == GROUP_RECORD){
        //#ifdef MYSQL_OPEN
        sprintf(command, "insert into zzy_chat_group_history values ('%s','%s','%s')",
        recv_pack->data.send_name,recv_pack->data.recv_name,recv_pack->data.mes);
    }else if(flag == USERS_RECORD){
        sprintf(command, "insert into zzy_chat_user_history values('%s','%s','%s')",
                recv_pack->data.send_name,recv_pack->data.recv_name,recv_pack->data.mes);   
        //printf("用户聊天记录存入成功!!!!\n");
    }

    //对数据库执行 SQL 语句
        rc = mysql_real_query(&mysql, command, strlen(command));     
    
        if (rc != 0) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            return ;
        }
    
        printf("the message get into the mysql!!!\n");
        //#else
        //#endif
}

//客户端请求历史记录，通过查询，把mysql数据库中的信息导出，发送给客户端
void send_record(PACK *recv_pack)
{
    char send_name[MAX_CHAR];
    char recv_name[MAX_CHAR];
    
        /* char mes[MAX_CHAR*2]; */
        PACK *pack_send_record_t = (PACK *)malloc(sizeof(PACK));
        strcpy(send_name,recv_pack->data.send_name);
        strcpy(recv_name,recv_pack->data.mes);
    
        //寻找消息===========================================================================
        //查找群聊天记录
        if(recv_pack->type == GROUP_RECORD){
            sprintf(command,"select mes from zzy_chat_group_history where recv_name = '%s' ",
                      recv_pack->data.mes);            
        }
        //查看好友聊天记录
        if(recv_pack->type == USERS_RECORD){
            sprintf(command,"select mes from zzy_chat_user_history where send_name = '%s' and recv_name = '%s' or send_name = '%s' and recv_name = '%s'",
                    send_name,recv_name,recv_name,send_name);
        }

        //执行
        rc = mysql_real_query(&mysql, command, strlen(command));
    
        if (0 != rc) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            return ;
        }
    
        //获取查询结果
        res = mysql_store_result(&mysql);                             
    
        if (NULL == res) {
             printf("mysql_restore_result(): %s\n", mysql_error(&mysql));
            return ;
        }

        //行数
        int rows = mysql_num_rows(res);                                    
            printf("The total rows is: %d\n", rows);                        
        //列数
        int fields = mysql_num_fields(res); 
            printf("The total fields is: %d\n", fields);  

        //寻找发送人===========================================================================
        //查找群
        if(recv_pack->type == GROUP_RECORD){
            sprintf(command,"select send_name from zzy_chat_group_history where recv_name = '%s' ",
                        recv_pack->data.mes);            
        }
        //查找好友
        if(recv_pack->type == USERS_RECORD){
            sprintf(command,"select send_name from zzy_chat_user_history where send_name = '%s' and recv_name = '%s' or send_name = '%s' and recv_name = '%s'",
                send_name,recv_name,recv_name,send_name);
        }


        //执行
        rc_man = mysql_real_query(&mysql, command, strlen(command));
    
        if (0 != rc_man) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            return ;
        }
    
        //获取查询结果
        res_man = mysql_store_result(&mysql);                             
    
        if (NULL == res_man) {
            printf("mysql_restore_result(): %s\n", mysql_error(&mysql));
            return ;
        }
    
        int rows_man = mysql_num_rows(res);                                  //获取查询结果的行数  
            printf("The total rows_man is: %d\n", rows_man);                        
      
        int fields_man = mysql_num_fields(res);                              //获取查询结果的列数  
            printf("The total fields_man is: %d\n", fields_man); 
    
    //============================================================================================

    int i = 0;
    //发送查询到的数据
    while ((row = mysql_fetch_row(res)) && (row_t = mysql_fetch_row(res_man))) 
    {     
        pack_send_record_t->type = recv_pack->type;
        strcpy(pack_send_record_t->data.send_name,"server");
        strcpy(pack_send_record_t->data.recv_name,send_name);
        strcpy(pack_send_record_t->data.mes,row[i]);
        strcpy(pack_send_record_t->data.group_chat,row_t[i]);
        //strcpy(pack_send_record_t->data.mes+SIZE_PASS_NAME,row[3]);
        //strcpy(pack_send_record_t->data.mes+SIZE_PASS_NAME,row[2]);
        printf("---------------------------------------%s\n",pack_send_record_t->data.mes);
        send_pack(pack_send_record_t);
        usleep(100000);   
    }
    
    usleep(100000);
    printf("OK!\n");
    
    //发送包，表示已经传输完毕
    pack_send_record_t->type = MES_RECORD;
    strcpy(pack_send_record_t->data.send_name,"server");
    strcpy(pack_send_record_t->data.recv_name,send_name);
    strcpy(pack_send_record_t->data.mes,"end!!!\0");
    send_pack(pack_send_record_t);
 
    usleep(10000);
    free(pack_send_record_t);
}










//报错函数
//输出错误信息
void my_err(const char * err_string,int line)
{
    fprintf(stderr, "line:%d  ", line);
    perror(err_string);
    exit(1);
}
 
//根据姓名找到用户信息
INFO_USER *find_userinfor(char username_t[])
{ 
    int i;
    INFO_USER *p = user_infor;
    if(user_num == 0)  return NULL;
    for(i = 0;i<=user_num;i++)
    {
        //printf("\n  user  %s  ,%d\n",p->username,i);
        if(strcmp(p->username,username_t) == 0)
            return p;
        p = p->next;
    }
    if(i == user_num+1) 
        return NULL;
}
 
 
 
//关闭服务器前 关闭服务器的socket
void signal_close(int i)
{
    //#ifdef LOG
        close(log_file_fd);
    //#else
    //#endif
    
    write_infor(); 
    mysql_close();
    printf("服务器已经关闭\n");
    exit(1);
}
 
 
//发送包，把包的内容赋值到发送包的数组
void send_pack(PACK *pack_send_pack_t)
{
    pthread_mutex_lock(&mutex);  
    printf("---\n");
    memcpy(&(m_pack_send[m_send_num++]),pack_send_pack_t,sizeof(PACK));
    pthread_mutex_unlock(&mutex);  
}
 
 
//根据参数信息发送包
void send_pack_memcpy_server(int type,char *send_name,char *recv_name,int sockfd1,char *mes)
{
    PACK pack_send_pack;
    time_t timep;
    pack_send_pack.type = type;
    strcpy(pack_send_pack.data.send_name,send_name);
    strcpy(pack_send_pack.data.recv_name,recv_name);
    memcpy(pack_send_pack.data.mes,mes,MAX_CHAR*2); 
    time(&timep);
    //pack_send_pack.data.time = timep;
    printf("sockfd1:%d\n", sockfd1);
    if(send(sockfd1,&pack_send_pack,sizeof(PACK),0) < 0){
        my_err("send",__LINE__);
    }
}
 
 
/* //判断客户端登录密码每是否正确
int passward_judge(int id,char *log_pass)
{
        if(strcmp(user_infor[id].password,log_pass) == 0){
            //printf("\n\n%s************%s\n\n",user_infor[id].password,log_pass);
            return 0;
        }
    return 1;
} */
 
 
//判断用户信息中是否有当前名字的信息
int judge_usename_same(char username_t[])
{
    int i;
    if(user_num == 0)  return 1;
    for(i=1;i<=user_num;i++)
    {
        if(strcmp(user_infor[i].username,username_t) == 0)
            return 0;
    }
    if(i == user_num+1) 
        return 1;
}
 
 
//给定一个用户，从用户信息删除好友
int del_friend_infor(INFO_USER *p,char friend_name[]) 
{
    int id_1;
    for(int i = 0 ;i < p->friends_num;i++){
        if(strcmp(p->friends[i],friend_name) == 0){   
            id_1 = i;
            break;
        }
    }
        for(int i = id_1;i < p->friends_num;i++){
            strcpy(p->friends[i],p->friends[i+1]);
        }
    p->friends_num--;
}
 
 
//给定用户信息，从用户信息中删除组
void del_group_from_user(char *username,char *groupname)
{
    INFO_USER *p_user = find_userinfor(username);
    for(int i = 0;i < p_user->group_num;i++)
    {
        if(strcmp(p_user->group[i].group_name,groupname) == 0)
        {
            for(int j = i ;j < p_user->group_num ;j++)
            {
                strcpy(p_user->group[j].group_name,p_user->group[j+1].group_name);
            }
            p_user->group_num--;
        }
    }
}
 
 
//给定组名，找到信息在数组中位置
INFO_GROUP *find_groupinfor(char groupname_t[])
{
    INFO_GROUP *p = group_infor;
    int i;
    if(group_num == 0)  return NULL;
    for(i = 0;i < group_num;i++)
    {
        if(strcmp(p->group_name,groupname_t) == 0)
            return p;
    }
    if(i == group_num) 
        return NULL;
}
 
 
//给定文件名，返回信息在数组中位置
int find_fileinfor(char *filename)
{
    for(int i=1 ;i <= m_file_num ;i++)
    {
        if(strcmp(filename , m_infor_file[i].file_name) == 0)
        {
            return i;
        }
    }
    return 0;
}





//显示包=======================================================================================================================
//发送包 
void print_send_pack()
{
    for(int i=1;i<=m_send_num;i++)
    {
        printf("********%d*********\n", i);
        printf("type      :%d\n", m_pack_send[i].type);
        printf("send_name :%s\n", m_pack_send[i].data.send_name);
        printf("recv_name :%s\n", m_pack_send[i].data.recv_name);
        printf("send_fd   :%d\n", m_pack_send[i].data.send_fd);
        printf("recv_fd   :%d\n", m_pack_send[i].data.recv_fd);
        printf("mes       :%s\n", m_pack_send[i].data.mes);
        printf("mes_int   :%d\n", m_pack_send[i].data.mes_int);
        printf("***********************\n\n\n");
    }
}

//打印群组信息
void print_infor_group()
{
    for(int i=1;i<=group_num;i++)
    {
        printf("\n\n********%d*********\n", i);
        printf("group_name  :%s\n", group_infor[i].group_name);
        printf("group_num   :%d\n", group_infor[i].member_num);
        for(int j=1 ;j<=group_infor[i].member_num;j++)
        printf("*%s\n", group_infor[i].member_name[j]);
        printf("***********************\n\n\n");
    }
}
 
//打印用户信息
void print_infor_user()
{
    for(int i=1;i<=user_num;i++)
    {
        printf("\n*****user***%d********* \n", i);
        printf("user_name  :%s\n", user_infor[i].username);
        printf("int  statu :%d\n", user_infor[i].statu);
        printf("friends_num:%d\n", user_infor[i].friends_num);
        for(int j=1 ;j<=user_infor[i].friends_num;j++)
        printf("*%s\n", user_infor[i].friends[j]);
 
        printf("user_num   :%d\n", user_infor[i].group_num);
        for(int j=1 ;j<=user_infor[i].group_num;j++)
        printf("*%s\n", user_infor[i].group[j]);
        printf("***********************\n\n\n");
 
    }
}

//打印传送文件进度
void print_infor_file()
{
    pthread_mutex_lock(&mutex_check_file);  
    for(int i=1;i<=m_file_num;i++)
    {  
        printf("\n\n****file***%d*********\n", i);
        printf("file_name       :%s\n", m_infor_file[i].file_name);
        printf("send_name       :%s\n", m_infor_file[i].file_send_name);
        printf("recv_name       :%s\n", m_infor_file[i].file_recv_name);
        printf("file_size       :%d\n", m_infor_file[i].file_size);
        printf("file_size_now   :%d\n", m_infor_file[i].file_size_now);
        printf("flag            :%d\n", m_infor_file[i].flag);
        printf("***********************\n\n\n");
 
    }
    pthread_mutex_unlock(&mutex_check_file);  
}
