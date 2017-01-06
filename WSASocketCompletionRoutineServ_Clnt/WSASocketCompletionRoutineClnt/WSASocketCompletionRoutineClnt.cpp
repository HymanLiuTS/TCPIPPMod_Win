// WSASocketCompletionRoutineClnt.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include <WinSock2.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 30

void ErrorHandler(const char* message);
void CALLBACK CompletionRoutine(DWORD dwError,DWORD szRecvBytes,LPWSAOVERLAPPED lpOverlapped,DWORD flags);

char buf[BUF_SIZE]="Hello world";
DWORD recvBytes = 0;
int _tmain(int argc, _TCHAR* argv[])
{
	SOCKET servSock;
	SOCKADDR_IN servAddr;

	WSAOVERLAPPED overLapped;
	HANDLE hEvent;
	WSABUF wsaBuf;
	DWORD ret,flags=0;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);



	//创建支持IO复用的套接字
	servSock=WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
	if(servSock==INVALID_SOCKET)
		ErrorHandler("WSASocket Error");

	memset(&servAddr,0,sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	servAddr.sin_port=htons(atoi("8888"));

	connect(servSock,(SOCKADDR*)&servAddr,sizeof(servAddr));

	memset(&overLapped,0,sizeof(overLapped));
	wsaBuf.buf=buf;
	wsaBuf.len=BUF_SIZE;
	hEvent=WSACreateEvent();
	overLapped.hEvent=hEvent;

	if(WSASend(servSock,&wsaBuf,1,&recvBytes,flags,&overLapped,CompletionRoutine)==SOCKET_ERROR)
	{
		if(GetLastError()==WSA_IO_PENDING)
			puts("background data send");
	}
	ret=WSAWaitForMultipleEvents(1,&hEvent,false,WSA_INFINITE,true);
	if(ret==WAIT_IO_COMPLETION)
		puts("Overlapped I/O Compeleted");
	else
		ErrorHandler("WSASend Error");

	WSACloseEvent(hEvent);
	closesocket(servSock);
	WSACleanup();
	return 0;
}

void CALLBACK CompletionRoutine(DWORD dwError,DWORD szRecvBytes,LPWSAOVERLAPPED lpOverlapped,DWORD flags)
{
	if(dwError!=0)
	{
		ErrorHandler("CompletionRoutine Error");
	}
	else
	{
		recvBytes=szRecvBytes;
		printf("Send Message:%s\n",buf);
	}
}

void ErrorHandler(const char* message)
{
	fputs(message,stderr);
	fputc('/n',stderr);
	exit(1);
}
