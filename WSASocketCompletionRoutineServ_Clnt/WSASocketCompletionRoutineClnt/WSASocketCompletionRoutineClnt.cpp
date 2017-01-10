#include "stdafx.h"
#include "winsock2.h"
#include "stdio.h"
#include "process.h"
//ws2_32.lib库
#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 1024
void ErrorHandler(char* message);

int _tmain(int argc, _TCHAR* argv[])
{
	SOCKET sock;
	SOCKADDR_IN addr;
	WSADATA wsaData;
	char buf[BUF_SIZE];
	int strLen,readLen;
	//通过主函数的参数传入IP地址和端口号
	if(argc!=3)
	{
		printf("Usage %s <addr> <port>\n",argv[0]);
		//exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
		ErrorHandler("WSAStartup() error");

	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock==INVALID_SOCKET)
		ErrorHandler("socket() error");

	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	addr.sin_port=htons(atoi("8888"));

	if(connect(sock,(SOCKADDR*)&addr,sizeof(addr))==SOCKET_ERROR)
		ErrorHandler("connect() error");

	while (1)
	{
		fputs("Input Message(Q to quit):",stdout);
		fgets(buf,BUF_SIZE,stdin);
		if(!strcmp(buf,"q\n")||!strcmp(buf,"Q\n"))
			break;

		strLen=strlen(buf);

		send(sock,buf,strLen,0);
		readLen=0;

		while(1)
		{
			readLen+=recv(sock,&buf[readLen],BUF_SIZE-1,0);
			if(readLen>=strLen)
				break;
		}
		buf[strLen]=0;
		printf("the message from server:%s\n",buf);
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}

//用于错误信息打印
void ErrorHandler(char* message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}

