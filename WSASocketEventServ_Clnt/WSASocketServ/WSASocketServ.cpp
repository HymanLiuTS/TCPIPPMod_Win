// WSASocketServ.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 30
#define SOCKS_SIZE 64

void ErrorHandler(const char* message);
void CompressSocks(SOCKET socks[],int pos,int size);
void CompressOverlappeds(WSAOVERLAPPED peds[],int pos,int size);

int _tmain(int argc, _TCHAR* argv[])
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	SOCKET servSock,clntSock;
	SOCKADDR_IN servAddr,clntAddr;
	int clntAddrSz;

	char buf[BUF_SIZE];

	SOCKET socks[SOCKS_SIZE];
	HANDLE events[SOCKS_SIZE];
	unsigned long ul=1; 
	int clntSockIndex;

	WSABUF wsaBuf;
	DWORD recvBytes,sendBytes,flags=0;
	WSAOVERLAPPED rdOverLappeds[SOCKS_SIZE];
	WSAOVERLAPPED wrOverLappeds[SOCKS_SIZE];

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
	
	//设置socket为非阻塞模式
	ioctlsocket(servSock,FIONBIO,(unsigned long *)&ul);
	clntSockIndex = 0;
	while(1)
	{
		clntAddrSz=sizeof(clntAddr);
		clntSock=accept(servSock,(SOCKADDR*)&clntAddr,&clntAddrSz);
		if(clntSock!=INVALID_SOCKET)
		{
			socks[clntSockIndex]=clntSock;
			//存储用于读的WSAOVERLAPPED变量
			HANDLE hRdEvent=WSACreateEvent();
			WSAOVERLAPPED rdOverLapped;
			memset(&rdOverLapped,0,sizeof(rdOverLapped));
			rdOverLapped.hEvent=hRdEvent;
			rdOverLappeds[clntSockIndex]=rdOverLapped;
			//存储用于写的WSAOVERLAPPED变量
			HANDLE hWrEvent=WSACreateEvent();
			WSAOVERLAPPED wrOverLapped;
			memset(&wrOverLapped,0,sizeof(wrOverLapped));
			wrOverLapped.hEvent=hWrEvent;
			wrOverLappeds[clntSockIndex]=wrOverLapped;

			clntSockIndex += 1;
		}

		for (int i=0;i<clntSockIndex;i++)
		{
			memset(&wsaBuf,0,sizeof(wsaBuf));
			wsaBuf.buf=buf;
			wsaBuf.len=BUF_SIZE;

			if(WSARecv(socks[i],&wsaBuf,1,&recvBytes,&flags,&rdOverLappeds[i],NULL)==SOCKET_ERROR)
			{
				if(WSAGetLastError()==WSA_IO_PENDING)
				{
					puts("Backgroud data recv");
					//当有客户端连接后，下面这句代码会发生阻塞，这个服务端一次只能和一个客户端进行会话
					WSAWaitForMultipleEvents(1,&rdOverLappeds[i].hEvent,true,WSA_INFINITE,false);
					WSAGetOverlappedResult(socks[i],&rdOverLappeds[i],&recvBytes,false,NULL);
				}
				else
				{
					puts("WSAGetLastError error");
					continue;
				}
			}
			if(recvBytes==0)
			{
				closesocket(socks[i]);
				CompressSocks(socks,i,SOCKS_SIZE);
				CompressOverlappeds(wrOverLappeds,i,SOCKS_SIZE);
				CompressOverlappeds(rdOverLappeds,i,SOCKS_SIZE);
				clntSockIndex --;
				i--;
				continue;
			}
			wsaBuf.buf=buf;
			wsaBuf.len=recvBytes;
			if(WSASend(socks[i],&wsaBuf,1,&sendBytes,flags,&wrOverLappeds[i],NULL)==SOCKET_ERROR)
			{
				if(WSAGetLastError()==WSA_IO_PENDING)
				{
					puts("Backgroud data send");
					WSAWaitForMultipleEvents(1,&wrOverLappeds[i].hEvent,true,WSA_INFINITE,false);
					WSAGetOverlappedResult(socks[i],&wrOverLappeds[i],&sendBytes,false,NULL);
				}
				else
				{
					puts("WSAGetLastError error");
					continue;
				}
			}
		}

	}

	closesocket(servSock);
	WSACleanup();
	return 0;
}
void CompressSocks(SOCKET socks[],int pos,int size)
{
	while(pos<size-1)
	{
		socks[pos]=socks[pos+1];
	}
}
void CompressOverlappeds(WSAOVERLAPPED peds[],int pos,int size)
{
	while(pos<size-1)
	{
		peds[pos]=peds[pos+1];
	}
}

void ErrorHandler(const char* message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(0);
}
