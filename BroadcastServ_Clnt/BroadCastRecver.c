#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<WinSock2.h>


#define BUF_SIZE 1024

#pragma comment(lib,"ws2_32.lib")

void error_handling(const char* message);
int main(int argc,char* argv[])
{
	WSADATA wsaData;
	SOCKET sock;
	SOCKADDR_IN addr;
	char message[BUF_SIZE];
	int strLen;
	if(argc!=2)
	{
		printf("Usage %s <address> <port>\n",argv[0]);
		//exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2),&wsaData)==SOCKET_ERROR)
		error_handling("WSAStartup() error");

	sock=socket(AF_INET,SOCK_DGRAM,0);
	if(sock==INVALID_SOCKET)
		error_handling("socket() error");

	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(atoi("9190"));

	if(bind(sock,(SOCKADDR*)&addr,sizeof(addr))==SOCKET_ERROR)
		error_handling("bind() error");

	while(1)
	{
		strLen = recvfrom(sock,message,BUF_SIZE-1,0,NULL,NULL);
		if(strLen<=0)
			break;
		message[strLen]=0;
		fputs(message,stdout);
	}
	closesocket(sock);
	return 0;
}

void error_handling(const char* message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}