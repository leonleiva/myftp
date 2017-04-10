#include "func.h"

int main(int argc,char *argv[])
{
	if(argc!=4)
	{
		printf("error argc\n");
		return -1;
	}
	int num=atoi(argv[3]);
	pchild p=(pchild)calloc(num,sizeof(child));
	make_child(p,num);//创建子进程并初始化子进程数据结构
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server;
	memset(&server,0,sizeof(struct sockaddr_in));
	server.sin_family=AF_INET;
	server.sin_port=htons(atoi(argv[2]));
	server.sin_addr.s_addr=inet_addr(argv[1]);
	bind(sfd,(struct sockaddr*)&server,sizeof(struct sockaddr_in));
	listen(sfd,num);
	//注册sfd及每个子进程的管道的另一端
	int epfd=epoll_create(1);
	struct epoll_event event;
	memset(&event,0,sizeof(struct epoll_event));
	event.events=EPOLLIN;
	event.data.fd=sfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);
	int i;
	for(i=0;i<num;++i)
	{
		memset(&event,0,sizeof(struct epoll_event));
		event.events=EPOLLIN;
		event.data.fd=p[i].fdw;
		epoll_ctl(epfd,EPOLL_CTL_ADD,p[i].fdw,&event);
	}
	struct epoll_event *events=(struct epoll_event *)calloc(num+1,sizeof(struct epoll_event));
	int retval,new_fd,j,flag;
	while(1)
	{
		memset(events,0,sizeof(struct epoll_event)*(num+1));
		retval=epoll_wait(epfd,events,num+1,-1);
		for(i=0;i<retval;++i)
		{
			if(sfd==events[i].data.fd)
			{
				new_fd=accept(sfd,NULL,NULL);
				for(j=0;j<num;++j)
				{
					if(!p[j].busy)
					{
						send_fd(p[j].fdw,new_fd);
						p[j].busy=1;
						close(new_fd);
						break;
					}
				}
			}
			for(j=0;j<num;++j)
			{
				if(p[j].fdw==events[i].data.fd)
				{
					read(p[j].fdw,&flag,sizeof(flag));
					p[j].busy=0;
				}
			}
		}
	}
	close(sfd);
	return 0;
}
