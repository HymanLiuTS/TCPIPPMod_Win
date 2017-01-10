// WSASocketCompletionRoutineServ.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include <WinSock2.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 1024


void CALLBACK ReadCompletionRoutine(DWORD dwError,DWORD szRecvBytes,LPWSAOVERLAPPED lpOverlapped,DWORD flags);
void CALLBACK WriteCompletionRoutine(DWORD dwError,DWORD szSendBytes,LPWSAOVERLAPPED lpOverlapped,DWORD flags);
void ErrorHandler(char* message);

typedef struct  
{
	SOCKET hClntSock;
	char buf[BUF_SIZE];
	WSABUF wsaBuf;
}IO_DATA,*LPIO_DATA;


int _tmain(int argc, _TCHAR* argv[])
{
	SOCKET servSock,clntSock;
	SOCKADDR_IN servAddr,clntAddr;
	int clntAddrSz;

	unsigned long mode=1;

	WSAOVERLAPPED ovLap;
	IO_DATA ioData;
	DWORD recvBytes,flagInfo;
	HANDLE hEvent;

	WSAData wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);

	servSock=WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
	ioctlsocket(servSock,FIONBIO,&mode);

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
		SleepEx(100,TRUE);
		clntAddrSz=sizeof(clntAddr);
		clntSock=accept(servSock,(SOCKADDR*)&clntAddr,&clntAddrSz);
		if(clntSock==INVALID_SOCKET)
		{
			if(GetLastError()==WSAEWOULDBLOCK)
				continue;
			else
				ErrorHandler("accept error");
		}		puts("client connected...");		memset(&ovLap,0,sizeof(WSAOVERLAPPED));
		ioData.hClntSock=clntSock;
		(ioData.wsaBuf).buf=ioData.buf;
		(ioData.wsaBuf).len=BUF_SIZE;		ovLap.hEvent=(HANDLE)(&ioData);				puts("begin WSARecv...");		WSARecv(clntSock,&(ioData.wsaBuf),1,&recvBytes,&flagInfo,&ovLap,ReadCompletionRoutine);	}
	closesocket(servSock);
	WSACleanup();
	return 0;
}

void CALLBACK WriteCompletionRoutine(DWORD dwError,DWORD szSendBytes,LPWSAOVERLAPPED lpOverlapped,DWORD flags)
{
	puts("entry WriteCompletionRoutine...");
	DWORD recvByte;
	DWORD flagInfo=0;
	puts("entry WriteCompletionRoutine...");
	if(dwError!=NULL)
		ErrorHandler("ReadCompletionRoutine error");
	LPIO_DATA lpioData=(LPIO_DATA)lpOverlapped->hEvent;
	SOCKET clntSock=lpioData->hClntSock;
	LPWSABUF lpwsaBuf=&(lpioData->wsaBuf);
	lpwsaBuf->len=BUF_SIZE;
	
	WSARecv(clntSock,lpwsaBuf,1,&recvByte,&flagInfo,lpOverlapped,ReadCompletionRoutine);
	
}

void CALLBACK ReadCompletionRoutine(DWORD dwError,DWORD szRecvBytes,LPWSAOVERLAPPED lpOverlapped,DWORD flags)
{
	puts("entry ReadCompletionRoutine...");
	if(dwError!=NULL)
		ErrorHandler("ReadCompletionRoutine error");
	LPIO_DATA lpioData=(LPIO_DATA)lpOverlapped->hEvent;
	SOCKET clntSock=lpioData->hClntSock;
	LPWSABUF lpwsaBuf=&(lpioData->wsaBuf);
	DWORD sendBytes;
	if(szRecvBytes==0)
	{
		closesocket(clntSock);
		free(lpioData);
		free(lpOverlapped);
	}
	else
	{
		lpwsaBuf->len=szRecvBytes;
		WSASend(clntSock,lpwsaBuf,1,&sendBytes,0,lpOverlapped,WriteCompletionRoutine);
	}
}

void ErrorHandler(char* message)
{
	fputs(message,stderr);
	fputc('/n',stderr);
	exit(1);
}

