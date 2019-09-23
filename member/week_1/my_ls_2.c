#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include<limits.h>
#include<dirent.h>
#include<grp.h>
#include<pwd.h>
#include<errno.h>
//#include<ctime>

#define PRINT_RED(s)    printf("\033[0;31m%-s\033[0;39m",s);
#define PRINT_GREEN(s)    printf("\033[0;32m%-s\033[0;39m",s);
#define PRINT_YELLOW(s)    printf("\033[0;33m%-s\033[0;39m",s);
#define PRINT_BLUE(s)    printf("\033[0;34m%-s\033[0;39m",s);
#define PRINT_PURPLE(s)    printf("\033[0;35m%-s\033[0;39m",s);/*紫色*/

#define PARAM_NONE 0    //无参数
#define PARAM_A    1    //-a：显示所有文件
#define PARAM_L    2    //-l：一行只显示一个文件的详细信息
#define PARAM_R    4    //-R:递归显示指定目录下所有的目录和目录中的文件
#define MAXROWLEN  80   //一行现实最多字符数

int g_leave_len = MAXROWLEN; //无参数
int g_maxlen;

void display(int flag,char *pathname);

//报错函数
void my_err(const char *err_string,int line){
    fprintf(stderr,"line:%d",line);
    perror(err_string);
    //exit(1);
}

//按照不同颜色打印
void print_bycolor(struct stat buf,char *name){
    if(S_ISDIR(buf.st_mode)){
        PRINT_BLUE(name);
    }else if(buf.st_mode & S_IXUSR){
        PRINT_GREEN(name);
    }else if(S_ISLNK(buf.st_mode)){
        PRINT_PURPLE(name);
    }
    else{
        printf("%s",name);
    }
}


//获取文件属性并打印
void display_arttribute(struct stat buf,char *name){
    char buf_time[32];
    struct passwd *psd; //从结构体中获取文件所有者的用户名
    struct group  *grp; //从该结构体中获取文件所有者所属组的组名

    
    //获取并且打印文件类型
    if(S_ISLNK(buf.st_mode)){
        printf("l");
    }else if(S_ISDIR(buf.st_mode)){
        printf("d");
    }else if(S_ISREG(buf.st_mode)){
        printf("-");
    }else if(S_ISCHR(buf.st_mode)){
        printf("c");
    }else if(S_ISBLK(buf.st_mode)){
        printf("b");
    }else if(S_ISFIFO(buf.st_mode)){
        printf("f");
    }else if(S_ISSOCK(buf.st_mode)){
        printf("s");
    }


    //获取并打印文件所有者的权限
    if(buf.st_mode & S_IWUSR){
        printf("w");
    }else{
        printf("-");
    }
    if(buf.st_mode & S_IRUSR){
        printf("r");
    }else{
        printf("-");
    }
    if(buf.st_mode & S_IXUSR){
        printf("x");
    }else{
        printf("-");
    }
    

    //获取并打印与文件所有者同组的用户对该文件的操作权限
    if(buf.st_mode & S_IWGRP){
            printf("w");
        }else{
            printf("-");
        }
        if(buf.st_mode & S_IRGRP){
            printf("r");
        }else{
            printf("-");
        }
        if(buf.st_mode & S_IXGRP){
            printf("x");
        }else{
            printf("-");
        }
    
    //获取并打印其他用户对该文件的操作权限
    if(buf.st_mode & S_IWOTH){
            printf("w");
        }else{
            printf("-");
        }
        if(buf.st_mode & S_IROTH){
            printf("r");
        }else{
            printf("-");
        }
        if(buf.st_mode & S_IXOTH){
            printf("x");
        }else{
           printf("-");
        }
    
    printf("\t");
    
    //根据uid和gid获取文件所有者的用户名和组名
    psd = getpwuid(buf.st_uid);
    grp = getgrgid(buf.st_gid);
    printf("%5lu ",buf.st_nlink);     //打印链接数目
    printf("%-10s",psd->pw_name);     //打印用户名
    printf("%-10s",grp->gr_name);     //打印用户组名

    printf("%6ld",buf.st_size);       //打印文件大小
    strcpy(buf_time,ctime(&buf.st_mtime));
    buf_time[strlen(buf_time) - 1] = '\0'; //去掉换行符
    printf(" %s",buf_time);           //打印文件时间信息
}


//在没有使用-l选项时，打印一个文件名，打印时上下行对齐
void display_single(struct stat buf, char *name){
    int i,len;

    //如果本行不足以打印一个文件名，则换行
    if(g_leave_len < g_maxlen){
        printf("\n");
        g_leave_len = MAXROWLEN;
    }

    len = g_maxlen - strlen(name);
    print_bycolor(buf,name);
    //printf("%-s",name);

    for(i = 0;i < len;i++){
        printf(" ");
    }
    printf(" ");
    //下面的2指示空两格
    g_leave_len -= (g_maxlen + 2);
}


// 打印所读取文件的信息
void display_dir2(DIR* ret_opendir)
{
    int i = 0;
    //printf("\n1\n");
    struct dirent* ret_readdir = NULL; // 定义readdir函数返回的结构体变量
    while((ret_readdir = readdir(ret_opendir))) // 判断是否读取到目录尾
    {
        //printf("222");
        char* filename = ret_readdir->d_name; // 获取文件名  
        struct stat buf;
        lstat(filename,&buf);
        //display_single(buf,filename);
        if(i == 5){
            //printf("\n");
        }
        if(filename[0]!='.'){ // 不输出当前目录、上一级目录与隐藏文件 
            //printf("%s\t",ret_readdir->d_name); // 打印文件名
            display_single(buf,filename);
            i++;
        }
    }
    printf("\n");
    rewinddir(ret_opendir); // 非常重要，将文件流的指针拨回起始位置
    
    //puts("");
    //puts("");
}


// 处理错误
void error_printf(const char* funname)
{
    perror(funname);
    exit(EXIT_FAILURE);
}

// 读取路径下的文件
int a = 1;
void list_dir(const char* pathname)
{
    char nextpath[PATH_MAX+1];

    DIR* ret_opendir = opendir(pathname); // 打开目录"pathname"
    if(ret_opendir == NULL)
        error_printf("opendir");
    
    printf("%s:\n",pathname); // 显示pathname目录路径
    display_dir2(ret_opendir); // 显示pathname目录下所有非隐藏文件名称
    //printf("\nsssssssss%d\n",a++);
    
    struct dirent* ret_readdir = NULL; // 定义readdir函数返回的结构体变量
    while((ret_readdir = readdir(ret_opendir))) // 判断是否读取到目录尾
    {
        //printf("ssss");
        char* filename = ret_readdir->d_name; // 获取文件名

        int end = 0; // 优化显示路径（处理"./test/"与"./test"）
        while(pathname[end])
            end++;
        strcpy(nextpath,pathname);
        if(pathname[end-1] != '/')
            strcat(nextpath,"/");
        strcat(nextpath,filename);

        struct stat file_message = {}; // 定义stat函数返回的结构体变量
        int ret_stat = lstat(nextpath, &file_message); // 获取文件信息
        
        //printf("\npath = %s\nfilename = %s\n%daaaa%d\n",nextpath,filename,S_ISDIR(file_message.st_mode),filename[0]!='.');
        if(ret_stat == -1) // stat读取文件错误则输出提示信息
            printf("%s error!", filename);
        else if(S_ISDIR(file_message.st_mode)  && filename[0]!='.') // 筛选"."、".."与隐藏>
        {
            //printf("\n--%s--\n",filename);
            //printf("\n%s\n",nextpath);
            list_dir(nextpath);
        }
    }
    closedir(ret_opendir);
}









//根据命令行参数和完整路径名显示目标文件
//参数flag: 命令行参数
//参数pathname：包含了文件名和路径名


void display(int flag,char *pathname){
    struct stat buf;
    char name[NAME_MAX + 1];
    unsigned int i,j;
    //从路径中解析出文件名
    for(i = 0,j = 0;i < strlen(pathname);i++){
        if(pathname[i] == '/'){
            j = 0;;
            continue;
        }
        name[j++] = pathname[i];
    }
    name[j] = '\0';

    //用lstat而不是stat，更方便解析链接文件
    if(lstat(pathname,&buf) == -1){
        my_err("stat",__LINE__);
    }

    switch(flag){
        case PARAM_NONE: //没有-l和-a选项
            if(name[0] != '.'){
                display_single(buf,name);
            }
            break;
        case PARAM_A:
            display_single(buf,name);
            break;
        case PARAM_L:
        //case PARAM_R:
        //case PARAM_L + PARAM_R:
            if(name[0] != '.'){
                display_arttribute(buf,name);
                printf("  ");
                print_bycolor(buf,name);
                printf("\n");
                //printf(" %s\n",name);
            }
            break;
        case PARAM_A + PARAM_L:
        //case PARAM_A + PARAM_R:
        //case PARAM_A + PARAM_L + PARAM_R:
            display_arttribute(buf,name);
            printf("  ");
            print_bycolor(buf,name);
            printf("\n");
            //printf(" %-s\n",name);
            break;
        case PARAM_R:
            //printf("%s\n\n\n",name);
            if(S_ISDIR(buf.st_mode) && name[1] != '.') 
                list_dir(name);
            //if(S_ISDIR(buf.st_mode)  && name[0]!='.')
            //printf("\n--%s--\n",name);
            break;
        case PARAM_L + PARAM_R:
             if(name[0] != '.')
				       {
					       display_arttribute(buf,name);
					       /*if(flag_color == 1)
					       {
					     	  printf("\033[34m %s\033[0m\n", name);
					       }
					       else */   printf( " %-s\n",name);
				      
					       /*if(lflag == 1)
						{
				      			 printf(" -> ");
							 printf( "%s",pathname);
						}

					       printf( "\n");
				       }*/
    }
					break;
            //display_arttribute(buf,name);
            
        default:
            break;
    
}}

void display_dir(int flag_param,char *path){    //显示目录
    /*DIR *dir;
    struct dirent *ptr;
    //struct stat buf;
    int count = 0;
    char filenames[256][PATH_MAX+1],temp[PATH_MAX+1];
    
    //获取该目录下文件总数和最长的文件名
    dir  = opendir(path);
    if(dir == NULL){
        my_err("打开文件错误",__LINE__);
    }
    while((ptr = readdir(dir)) != NULL){
        if(g_maxlen < strlen(ptr->d_name))
            g_maxlen = strlen(ptr->d_name);
        count++;
    }
    closedir(dir);

    if(count > 256)
        my_err("目录下文件过多",__LINE__);

    int i,j,len = strlen(path);
    //获取该目录下所有文件名
    dir = opendir(path);
    for(i = 0;i < count;i++){
        ptr = readdir(dir);
        if(ptr == NULL){
            my_err("读取文件错误",__LINE__);
        }
        strncpy(filenames[i],path,len);
        filenames[i][len] = '\0';
        strcat(filenames[i],ptr->d_name);
        filenames[i][len+strlen(ptr->d_name)] = '\0';
    }
    

    //////////////////////////////////////////////////////////////////////////////
    //暂用冒泡
    for(i = 0;i < count-1;i++){
        for(j = 0;j < count-1-i;j++){
            if(strcmp(filenames[j],filenames[j+1]) > 0){
                strcpy(temp,filenames[j+1]);
                temp[strlen(filenames[j+1])] = '\0';
                strcpy(filenames[j+1],filenames[j]);
                filenames[j+1][strlen(filenames[j])] = '\0';
                strcpy(filenames[j],temp);
                filenames[j][strlen(temp)] = '\0';
            }
        }
    }
    
       for(int i = 0;i < count;i++){
            display(flag_param,filenames[i]);
       }

    closedir(dir);

    //如果命令行中没有一个-1选项，则打印一个换行符
    if((flag_param & PARAM_L) == 0){
        printf("\n");
    }*/
    char temp[PATH_MAX];
	strcpy(temp,path);
	DIR *dir;       //目录流
	struct dirent   *ptr;   //目录信息结构体
	int count  = 0;    //该目录下的文件总数


	char name[256];
	struct stat buf;    //存储文件信息的结构体
	char new_path [256];
	//获取该目录下文件总数和最长的文件名
	dir = opendir(path);
	printf( "\npath %s\n",path);
	if(dir == NULL)  my_err("opendir",__LINE__);
	else {
	while((ptr = readdir(dir)) != NULL)
	{
		if(g_maxlen < strlen(ptr->d_name))   //获取最长文件名
		{
			g_maxlen = strlen(ptr->d_name);
		}

		 count++;    //获取总数
	}}
	closedir(dir);
	char **filenames = (char **)malloc(sizeof(char *) * count);
	for(int i = 0;i < count;i++)
	{
		filenames[i] = (char *)malloc(sizeof(char) * PATH_MAX + 1);
	}


	int i,j,len = strlen(path);
	//获取 该 目录下的所有文件名

	dir = opendir(path);   //获取目录流
	for(i = 0;i < count;i++)
	{
		ptr = readdir(dir);

		if(ptr == NULL)   my_err("dir_ptr",__LINE__);
		strcpy(filenames[i],path);
                filenames[i][len] = '\0';
                strcpy(filenames[i],ptr->d_name);
                filenames[i][len+strlen(ptr->d_name)] = '\0';
	}

	//将文件 名 和 路径进行拼接
	for(int i = 0;i < count;i++)
	{
		int len = strlen(path);
		temp[len+1] = '\0';
		strcat(temp,filenames[i]);
		strcpy(filenames[i],temp);
		strcpy(temp,path);

	}


	//使用冒泡法进行排序,排序后文件名按 字母顺序存储于 filenames

	for(i = 0;i < count - 1;i++)
	{
		for(j = 0;j < count-i-1;j++)
		{
			if(strcmp(filenames[j],filenames[j+1]) > 0)  // j > j+1
			{
				strcpy(temp,filenames[j+1]);
				temp[strlen(filenames[j+1])]  = '\0';
				strcpy(filenames[j+1],filenames[j]);
				filenames[j+1][strlen(filenames[j])] = '\0';
				strcpy(filenames[j],temp);
				filenames[j][strlen(temp)] = '\0';
			}
		}
	}

	for(i = 0;i < count;i++)
	{
	//	if(strcmp(filenames[i],"") == 0) continue;
		display(flag_param,filenames[i]);
	}
	int k;
	if(flag_param & PARAM_R)
	{
		for(i = 0;i < count;i++)
		{
			if(lstat(filenames[i],&buf) == -1)
			{
				my_err("stat",__LINE__);
			}

			if(S_ISDIR(buf.st_mode))
			{
				        for(j = 0,k = 0;j < strlen(filenames[i]);j++)
					{
				                if(filenames[i][j] == '/')
              				        {
        		        		        k = 0;
              	 	 			        continue;
       					        }
     					        name[k++] = filenames[i][j];
    	  				}
		     		   name[k] = '\0';



				if(strcmp(name,".") == 0 || strcmp(name,"..") == 0)  continue;
				int len = strlen(filenames[i]);
				if(flag_param & PARAM_A)
				{
					if(filenames[i][strlen(filenames[i]) - 1] != '/')
					{
						filenames[i][len] = '/';
						filenames[i][len+1] = '\0';
					}
					display_dir(flag_param,filenames[i]);
					free(filenames[i]);
				}
				{
					if(name[0] != '.')
					{
						if(filenames[i][strlen(filenames[i]) - 1] != '/')
						{
							filenames[i][len] = '/';
							filenames[i][len+1] = '\0';
						}
						display_dir(flag_param,filenames[i]);
						free(filenames[i]);
					}
				}
			}

		}
	}

	closedir(dir);

	//如果命令中没有 -l 选项,打印一个换行符
	//当 flag 中 没有 l 选项时
	if((flag_param & PARAM_L) == 0)   printf( "\n");


}
//}
int main(int argc,char **argv){
        char path[PATH_MAX+1];
        char param[32];         //保存命令行参数，目标文件名和目录名不在这里
        int flag_param = PARAM_NONE;  //参数种类，即是否含有-l,-a选项
        int i,j,k,num;
        struct stat buf;
        
        
        //命令行分析，-l,-a,-al,-la选项
        j = 0;
        num = 0;
        for(i = 1;i < argc;i++){    //控制参数
            if(argv[1][0] == '-'){
                for(k = 1;k < strlen(argv[i]);k++,j++){
                    param[j] = argv[i][k];      //获取 - 后面的参数，保存到数组param中
                }
                num++;  //计算 - 的数目
            }
        }

        //除了a和l，如果含有其它选项则报错
        for(i = 0;i < j;i++){
            if(param[i] == 'a'){
                flag_param |= PARAM_A;
                continue;
            }else if(param[i] == 'l'){
                flag_param |= PARAM_L;
                continue;
            }else if(param[i] == 'R'){
                flag_param |= PARAM_R;
                continue;
            }
            else{
                printf("my_ls:invalid option = %c\n",param[i]);
                exit(1);
            }
        }
        param[j] = '\0';

        //没有输入文件名或目录，就显示当前目录
        if((num+1) == argc){
            strcpy(path,"./");
            path[2] = '\0';
            display_dir(flag_param,path);
            return 0;
        }

        i = 1;
        do{
            //如果不是目标文件名或目录，解析下一个命令行参数
            if(argv[i][0] == '-'){
                i++;
                continue;
            }else{
                strcpy(path,argv[i]);

                //如果目标文件或目录不存在，报错并退出程序
                if(stat(path,&buf) == -1){
                    my_err("stat",__LINE__);
                }
            }
            
            if(S_ISDIR(buf.st_mode)){   //argv[i]是一个目录
                    //如果目录的最后一个字符不是'/'，就加上'/'
                    if(path[strlen(argv[i]) - 1] != '/'){
                        path[strlen(argv[i])] = '/';
                        path[strlen(argv[i]+1)] = '\0';
                    }
                
                    else{
                        path[strlen(argv[i])] = '\0';
                    }

                    display_dir(flag_param,path);
                    i++;
            }
            else{   //argv[i]是一个文件
                display(flag_param,path);
                i++;
            }
        }while(i<argc);
            
        return 0;
}

