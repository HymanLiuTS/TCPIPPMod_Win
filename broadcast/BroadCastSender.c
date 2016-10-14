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
	int broadcs=1;

	if(argc!=3)
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
	addr.sin_addr.s_addr=htonl(atoi("255.255.255.255"));
	addr.sin_port=htons(atoi("9190"));

	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(const char*)&broadcs,sizeof(broadcs));

	while(1)
	{
		fputs("Input:",stdout);
		fgets(message,BUF_SIZE,stdin);
		if(!strcmp(message,"q\n")||
			!strcmp(message,"Q\n"))
			break;
		sendto(sock,message,BUF_SIZE,0,(SOCKADDR*)&addr,sizeof(addr));
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