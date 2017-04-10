#include "func.h"

int main(int argc,char *argv[])
{
		if(argc!=3)
		{
			printf("error args\n");
			return -1;
		}
		int sfd=socket(AF_INET,SOCK_STREAM,0);
		struct sockaddr_in server;
		server.sin_family=AF_INET;
		server.sin_port=htons(atoi(argv[2]));
		server.sin_addr.s_addr=inet_addr(argv[1]);
		connect(sfd,(struct sockaddr*)&server,sizeof(struct sockaddr_in));
		print(sfd);
		char input[1000];
		train t;
		int len;
		char buf[1000];
		const char *CMD[6]={"cd","pwd","ls","puts","remove","gets"};
		char cmd_name[1000];
		int cmd_no;
		while(1)
		{
			while(1)
			{
				memset(buf,0,sizeof(buf));
				if(!fgets(buf,sizeof(buf),stdin))
				{
					t.len=0;
					send(sfd,&t,4+t.len,0);
					goto end;
				}
				memset(input,0,sizeof(input));
				input_handle(buf,input);
				memset(cmd_name,0,sizeof(cmd_name));
				get_cmd(input,cmd_name);
				for(cmd_no=0;cmd_no<6;++cmd_no)
				{
					if(!strcmp(cmd_name,CMD[cmd_no]))
						break;
				}
				if(cmd_no>=6)
				{
					system("clear");
					printf("Have no this command!try again\n");
				}else{
					break;
				}	
			}		
			memset(buf,0,sizeof(buf));
			t.len=strlen(input);
			strcpy(t.buf,input);
			send(sfd,&t,4+t.len,0);
			switch(cmd_no)
			{
				case 0:print(sfd);break;
				case 1:print(sfd);break;
				case 2:print(sfd);break;
				case 3:send_file_client(sfd,input);break;
				case 4:print(sfd);break;
				case 5:recv_file_client(sfd);break;
			}		
		}
	end:
		close(sfd);
		return 0;
}
