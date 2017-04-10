#include "func.h"
//获取命令
void get_cmd(char *str,char *cmd_name)
{
	int i,j=0;
	for(i=0;i<strlen(str)&&str[i]!=' ';++i)
	{
			cmd_name[j++]=str[i];
	}
}

//获取路径名或文件名
void get_name(char *str,char *name)
{
		while(*str&&*str!=' ')
		{
			++str;
			continue;
		}
		if(!*str)
		{
			strcpy(name,".");
		}else{
			++str;
			strcpy(name,str);
		}
}

//cd
void change_dir(int new_fd,char *cmd)
{
	char path_name[1000]={0};
	get_name(cmd,path_name);
	chdir(path_name);
	print_work_dir(new_fd);
}

//pwd
void print_work_dir(int new_fd)
{
	train t;
	memset(&t,0,sizeof(train));
	getcwd(t.buf,sizeof(t.buf));
	t.len=strlen(t.buf);
	send(new_fd,&t,4+t.len,0);
}

//ls
void list_dir(int new_fd,char *cmd)
{
	char path_name[1000]={0};
	get_name(cmd,path_name);
	DIR *dir;
	dir=opendir(path_name);
	struct dirent *p;
	struct stat buf;
	int i;
	char abs_path[1000];
	train t;
	memset(&t,0,sizeof(train));
	while((p=readdir(dir))!=NULL)
	{
		if(!strcmp(p->d_name,".")||!strcmp(p->d_name,".."))
			continue;
		memset(abs_path,0,sizeof(abs_path));
		sprintf(abs_path,"%s%s%s",path_name,"/",p->d_name);
		memset(&buf,0,sizeof(struct stat));
		stat(abs_path,&buf);
		if(4==(buf.st_mode>>12&0xf))
			strcpy(t.buf+strlen(t.buf),"d ");
		else
			strcpy(t.buf+strlen(t.buf),"- ");
		strcpy(t.buf+strlen(t.buf),p->d_name);
		t.buf[strlen(t.buf)]='\n';
	}
	t.buf[strlen(t.buf)-1]='\0';
	t.len=strlen(t.buf);
	send(new_fd,&t,4+t.len,0);
	closedir(dir);
}

//recv_file
void recv_file_server(int new_fd)
{
	int len;
	char buf[1000];
	//接收文件名并新建文件
	recv(new_fd,&len,sizeof(len),0);
	memset(buf,0,sizeof(buf));
	recv(new_fd,buf,len,0);
	int fd=open(buf,O_RDWR|O_CREAT,0666);
	//接收数据并写入文件
	while(1)
	{
		recv(new_fd,&len,sizeof(len),0);
		if(len>0)
		{
			memset(buf,0,sizeof(buf));
			recv_cycle(new_fd,buf,len);
			write(fd,buf,len);
		}else{
			break;
		}
	}
	close(fd);
}

//send_file
void send_file_server(int new_fd,char *cmd)
{
	char file_name[1000]={0};
	get_name(cmd,file_name);
	train t;
	//发送文件名
	memset(&t,0,sizeof(train));
	t.len=strlen(file_name);
	strcpy(t.buf,file_name);
	send(new_fd,&t,4+t.len,0);
	//发送文件内容
	int fd=open(file_name,O_RDWR);
	int ret;
	while((t.len=read(fd,t.buf,sizeof(t.buf)))>0)
	{
		ret=send_cycle(new_fd,(char*)&t,4+t.len);
		if(-1==ret)//客户端断开停止发送数据
		{
		goto end;
		}
	}
	//发送结束
	t.len=0;
	send(new_fd,&t,4+t.len,0);
end:
	close(fd);
}

//remove_file
void remove_file(int new_fd,char *cmd)
{
	char file_name[1000]={0};
	get_name(cmd,file_name);
	char path_name[1000]={0};
	sprintf(path_name,"%s%s","./",file_name);
	remove(file_name);
	print_work_dir(new_fd);
}
