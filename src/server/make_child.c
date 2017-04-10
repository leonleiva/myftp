#include "func.h"

//创建子进程
void make_child(pchild p,int num)
{
	int fds[2],i;
	pid_t pid;
	for(i=0;i<num;++i)
	{
		socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
		pid=fork();
		if(!pid)
		{
			close(fds[1]);
			child_handle(fds[0]);
		}else{
			close(fds[0]);
			p[i].pid=pid;
			p[i].fdw=fds[1];
			p[i].busy=0;
		}
	}	
}

//子进程处理函数
void child_handle(int fdr)
{
	int new_fd,flag=1;
	int len;
	char buf[1000];
	const char *CMD[6]={"cd","pwd","ls","puts","remove","gets"};
	char cmd_name[1000];
	int cmd_no;
	signal(SIGPIPE,SIG_IGN);//捕捉SIGPIPE并忽略
	while(1)
	{
		recv_fd(fdr,&new_fd);
		print_work_dir(new_fd);
		while(1)
		{
			recv(new_fd,&len,sizeof(len),0);
			if(!len)
				break;
			memset(buf,0,sizeof(buf));
			recv(new_fd,buf,len,0);
			memset(cmd_name,0,sizeof(cmd_name));
			get_cmd(buf,cmd_name);
			cmd_no=-1;
			for(cmd_no=0;cmd_no<6;++cmd_no)
			{
				if(!strcmp(cmd_name,CMD[cmd_no]))
					break;
			}
			switch(cmd_no)
			{
				case 0:change_dir(new_fd,buf);break;
				case 1:print_work_dir(new_fd);break;
				case 2:list_dir(new_fd,buf);break;
				case 3:recv_file_server(new_fd);break;
				case 4:remove_file(new_fd,buf);break;
				case 5:send_file_server(new_fd,buf);break;
			}
		}
		write(fdr,&flag,sizeof(flag));
		close(new_fd);
	}
}
