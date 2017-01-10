//////////////////////////////////////////////////////////////////////////
//////File: Clnt.cpp
//////Author: Hyman
//////Date: 2016/12/13
//////Description: 多线程的socket客户端
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "winsock2.h"
#include "stdio.h"
#include "process.h"

//ws2_32.lib库
#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 30
void ErrorHandler(char* message);
unsigned WINAPI WriteHandler(void* param);
unsigned WINAPI ReadHandler(void* param);
//事件数组用来控制线程的读写
HANDLE evts[2];
//读取的标准输入的长度，用来接收客户端数据的最大值
int strLen=0;


int _tmain(int argc, _TCHAR* argv[])
{
	SOCKET sock;
	sockaddr_in addr;
	WSADATA wsaData;
	HANDLE hThreads[2];
	int strLen;
	//通过主函数的参数传入IP地址和端口号
	if(argc!=3)
	{
		printf("Usage %s <addr> <port>\n",argv[0]);
		exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2),&wsaData)==SOCKET_ERROR)
		ErrorHandler("WSAStartup() error");

	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock==INVALID_SOCKET)
		ErrorHandler("socket() error");

	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr(argv[1]);
	addr.sin_port=htons(atoi(argv[2]));

	if(connect(sock,(sockaddr*)&addr,sizeof(addr))==SOCKET_ERROR)
		ErrorHandler("connect() error");
	//创建线程用于写和读
	hThreads[0]=(HANDLE)_beginthreadex(NULL,0,WriteHandler,&sock,0,NULL);
	hThreads[1]=(HANDLE)_beginthreadex(NULL,0,ReadHandler,&sock,0,NULL);
	//创建事件，都为manual-reset模式，控制写的事件起初是有信号状态，读的事件起初是无信号状态
	evts[0]=CreateEvent(NULL,true,true,NULL);
	evts[1]=CreateEvent(NULL,true,false,NULL);

	WaitForMultipleObjects(2,hThreads,true,INFINITE);

	CloseHandle(evts[0]);
	CloseHandle(evts[1]);
	return 0;
}

//用于错误信息打印
void ErrorHandler(char* message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}

//用于写线程
unsigned WINAPI WriteHandler(void* param)
{
	char buf[BUF_SIZE];
	SOCKET sock=*((SOCKET*)(param));
	while(1)
	{
		WaitForSingleObject(evts[0],INFINITE);
		fputs("input:",stdout);
		memset(buf,0,BUF_SIZE);
		fgets(buf,BUF_SIZE,stdin);
		if(!strcmp(buf,"q\n")||!strcmp(buf,"Q\n"))
		{
			strLen=-1;
			SetEvent(evts[1]);
			break;
		}
		strLen=strlen(buf);
		send(sock,buf,strLen,0);
		ResetEvent(evts[0]);
		SetEvent(evts[1]);
	}
	return 0;
}

//用于读线程
unsigned WINAPI ReadHandler(void* param)
{
	char buf[BUF_SIZE];
	SOCKET sock=*((SOCKET*)(param));
	while(1)
	{
		int temp=0;
		WaitForSingleObject(evts[1],INFINITE);
		if(strLen==-1)
		{
			SetEvent(evts[0]);
			break;;
		}
		while(temp<strLen)
		{
			temp+=recv(sock,buf,BUF_SIZE,0);
		}
		buf[temp]=0;
		fputs("the meesgae from serv:",stdout);
		printf("%s",buf);
		ResetEvent(evts[1]);
		SetEvent(evts[0]);
	}
	return 0;
}

