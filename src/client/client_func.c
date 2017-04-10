#include "func.h"

//输入处理函数
void input_handle(const char *buf,char *input)
{
	while(*buf==' ')
	{
		++buf;
		continue;
	}
	while(*buf!='\n'&&*buf!=' ')
	{
		*input=*buf;
		++buf;
		++input;
	}
	if(*buf=='\n')
		return;
	while(*buf==' ')
	{
		++buf;
		continue;
	}
	*input++=' ';
	while(*buf!='\n'&&*buf!=' ')
	{	
		*input=*buf;
		++buf;
		++input;
	}
}

//获取命令
void get_cmd(char *str,char *cmd_name)
{
	int i,j=0;
	for(i=0;str[i]&&str[i]!=' ';++i)
	{
		cmd_name[j++]=str[i];
	}
}

//获取文件名
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

//print
void print(int sfd)
{
	int len;
	char buf[1000];
	recv(sfd,&len,sizeof(len),0);
	memset(buf,0,sizeof(buf));
	recv(sfd,buf,len,0);
	system("clear");
	printf("%s\n",buf);
}

//send_file
void send_file_client(int sfd,char *cmd)
{
	char file_name[1000]={0};
	get_name(cmd,file_name);
	system("clear");
	printf("upload begin\n");
	train t;
	//发送文件名
	memset(&t,0,sizeof(train));
	t.len=strlen(file_name);
	strcpy(t.buf,file_name);
	send(sfd,&t,4+t.len,0);
	//发送文件内容
	int fd=open(file_name,O_RDWR);
	int ret;
	while((t.len=read(fd,t.buf,sizeof(t.buf)))>0)
	{
		ret=send_cycle(sfd,(char*)&t,4+t.len);
		if(-1==ret)//客户端断开停止发送数据
		{
			goto end;
		}
	}
	//发送结束
	t.len=0;
	send(sfd,&t,4+t.len,0);
	printf("upload over\n");
end:
	close(fd);
}

//recv_file
void recv_file_client(int sfd)
{
	int len;
	char buf[1000];
	system("clear");
	printf("download begin\n");
	//接收文件名并新建文件
	recv(sfd,&len,sizeof(len),0);
	memset(buf,0,sizeof(buf));
	recv(sfd,buf,len,0);
	int fd=open(buf,O_RDWR|O_CREAT,0666);
	//接收数据并写入文件
	while(1)
	{
		recv(sfd,&len,sizeof(len),0);
		if(len>0)
		{
			memset(buf,0,sizeof(buf));
			recv_cycle(sfd,buf,len);
			write(fd,buf,len);
		}else{
			break;
		}
	}
	close(fd);
	printf("download over\n");
}
