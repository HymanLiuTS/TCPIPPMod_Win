// IOCPServ_Clnt.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include "process.h"
#include "stdlib.h"
#include "WinSock2.h"
#include "Windows.h"

#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 100
#define READ 3
#define WRITE 5

typedef struct  
{
	SOCKET hClntSock;
	SOCKADDR clntAdr;
}HANDLE_DATA,*LPHANDLE_DATA;

typedef struct 
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUF_SIZE];
	int rwMode;
}IO_DATA,*LPIO_DATA;

unsigned WINAPI ThreadMain(LPVOID CompletionPortIO);
void ErrorHandler(char* message);

int _tmain(int argc, _TCHAR* argv[])
{
	WSAData wsaData;
	HANDLE hComPort;
	SYSTEM_INFO sysInfo;
	LPIO_DATA ioInfo;
	LPHANDLE_DATA handleInfo;

	SOCKET servSock;
	SOCKADDR_IN servAddr;
	DWORD recvBytes,i,flags=0;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	
	hComPort=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);
	GetSystemInfo(&sysInfo);

	for(i=0;i<sysInfo.dwNumberOfProcessors;i++)
		_beginthreadex(NULL,0,ThreadMain,(LPVOID)hComPort,0,NULL);

	servSock=WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
	if(servSock==INVALID_SOCKET)
		ErrorHandler("WSASocket Error");

	memset(&servAddr,0,sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_port=htons(atoi("8888"));

	if(bind(servSock,(SOCKADDR*)&servAddr,sizeof(servAddr))==SOCKET_ERROR)
		ErrorHandler("bind error");

	if(listen(servSock,5)==SOCKET_ERROR)
		ErrorHandler("listen error");

	while(1)
	{
		SOCKET clntSock;
		SOCKADDR_IN clntAddr;
		int clntAddrSz;

		clntSock=accept(servSock,(SOCKADDR*)&clntAddr,&clntAddrSz);
		handleInfo=(LPHANDLE_DATA)malloc(sizeof(HANDLE_DATA));
		handleInfo->hClntSock=clntSock;
		memcpy(&(handleInfo->clntAdr),&clntAddr,sizeof(clntAddr));

		CreateIoCompletionPort((HANDLE)clntSock,hComPort,(DWORD)handleInfo,0);

		ioInfo=(LPIO_DATA)malloc(sizeof(IO_DATA));
		memset(&(ioInfo->overlapped),0,sizeof(OVERLAPPED));
		ioInfo->wsaBuf.len=BUF_SIZE;
		ioInfo->wsaBuf.buf=ioInfo->buffer;
		ioInfo->rwMode=READ;
		WSARecv(handleInfo->hClntSock,&(ioInfo->wsaBuf),1,&recvBytes,&flags,&(ioInfo->overlapped),NULL);
	}
	
	WSACleanup();
	return 0;
}

unsigned WINAPI ThreadMain(LPVOID CompletionPortIO)
{
	HANDLE hComPort=(HANDLE)CompletionPortIO;
	SOCKET sock;
	DWORD bytesTrans;
	LPHANDLE_DATA handleInfo;
	LPIO_DATA ioInfo;
	DWORD flags;

	while(1)
	{
		GetQueuedCompletionStatus(hComPort,&bytesTrans,(PULONG_PTR)&handleInfo,(LPOVERLAPPED*)&ioInfo,INFINITE);
		sock=handleInfo->hClntSock;

		if(ioInfo->rwMode==READ)
		{
			puts("message received!");
			if(bytesTrans==0)
			{
				closesocket(sock);
				free(handleInfo);
				free(ioInfo);
				continue;
			}
			memset(&(ioInfo->overlapped),0,sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len=bytesTrans;
			ioInfo->rwMode=WRITE;
			WSASend(sock,&(ioInfo->wsaBuf),1,NULL,0,&(ioInfo->overlapped),NULL);

			ioInfo=(LPIO_DATA)malloc(sizeof(IO_DATA));
			memset(&(ioInfo->overlapped),0,sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len=BUF_SIZE;
			ioInfo->wsaBuf.buf=ioInfo->buffer;
			ioInfo->rwMode=READ;
			WSARecv(sock,&(ioInfo->wsaBuf),1,NULL,&flags,&(ioInfo->overlapped),NULL);
		}
		else
		{
			puts("message sent");
			free(ioInfo);
		}
	}
	return 0;
}

void ErrorHandler(char* message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}

