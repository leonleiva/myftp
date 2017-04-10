#include "func.h"

//循环发送
int send_cycle(int sfd,char *buf,int len)
{
	int ret,total=0;
	while(total<len)
	{
		ret=send(sfd,buf+total,len-total,0);
		if(-1==ret)//客户端断开，引发SIGPIPE信号，send返回-1
		{
			printf("client close\n");
			return -1;
		}
		total+=ret;
	}
	return 0;
}

//循环接收
void recv_cycle(int sfd,char *buf,int len)
{
	int ret,total=0;
	while(total<len)
	{
		ret=recv(sfd,buf+total,len-total,0);
		total+=ret;
	}
}
