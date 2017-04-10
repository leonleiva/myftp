#ifndef __FUNC_H__
#define __FUNC_H__

#include "head.h"

//子进程数据结构
typedef struct{
	pid_t pid;
	int fdw;
	short busy;
}child,*pchild;

//数据包
typedef struct{
	int len;
	char buf[1000];
}train,*ptrain;

//公有函数
void send_fd(int fdw,int fd);
void recv_fd(int fdr,int *pfd);
int send_cycle(int sfd,char *buf,int len);
void recv_cycle(int sfd,char *buf,int len);

//服务器函数
void make_child(pchild p,int sum);
void child_handle(int fdr);
void get_cmd(char *str,char *cmd_name);
void get_name(char *str,char *name);
void change_dir(int new_fd,char *cmd);
void print_work_dir(int new_fd);
void list_dir(int new_fd,char *cmd);
void recv_file_server(int new_fd);
void send_file_server(int new_fd,char *cmd);
void remove_file(int new_fd,char *cmd);

//客户端函数
void input_handle(const char *buf,char *input);
void get_cmd(char *str,char *cmd_name);
void get_name(char *str,char *name);
void print(int sfd);
void recv_file_client(int sfd);
void send_file_client(int sfd,char *cmd);

#endif
