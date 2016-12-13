//////////////////////////////////////////////////////////////////////////
//////File: Serv.cpp
//////Author: Hyman
//////Date: 2016/12/13
//////Description: 多线程的socket服务端
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "winsock2.h"
#include "stdio.h"
#include "windows.h"
#include "process.h"
//引入ws2_32.lib库
#pragma comment(lib,"ws2_32.lib")

#define  BUF_SIZE 30
#define THREAD_SIZE 20

void ErrorHandler(char* message);
unsigned WINAPI ClntHandler(void* param);
//存放客户端socket的数组
SOCKET clntSocks[THREAD_SIZE];
HANDLE mtx;
//总的客户端连接的个数
int clntCnt=0;

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	SOCKET servSock,clntSock;
	SOCKADDR_IN servAddr,clntAddr;
	int clntAddrSz;
	
	//存储创建的线程句柄
	HANDLE hClntThreadHandles[THREAD_SIZE];
	
	//通过主函数的参数传入端口号
	if(argc!=2)
	{
		printf("Usage %s <port>\n",argv[0]);
		exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2),&wsaData)==SOCKET_ERROR)
		ErrorHandler("WSAStartup error");

	servSock=socket(AF_INET,SOCK_STREAM,0);
	if(servSock==INVALID_SOCKET)
		ErrorHandler("socket() error");

	memset(&servAddr,0,sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_port=htons(atoi(argv[1]));

	if(bind(servSock,(const sockaddr*)&servAddr,sizeof(servAddr))==SOCKET_ERROR)
		ErrorHandler("bind() error");

	listen(servSock,5);
	//创建互斥，初始状态为signaled，不属于任何线程
	mtx=CreateMutex(NULL,FALSE,NULL);
	while(clntCnt<THREAD_SIZE)
	{
		clntAddrSz=sizeof(clntAddr);
		clntSock=accept(servSock,(sockaddr*)&clntAddr,&clntAddrSz);

		WaitForSingleObject(mtx,INFINITE);
		clntSocks[clntCnt]=clntSock;
		ReleaseMutex(mtx);
		//开启线程
		hClntThreadHandles[clntCnt++]=(HANDLE)_beginthreadex(NULL,0,ClntHandler,&clntSock,0,NULL);
		
	}
	//等待线程结束
	WaitForMultipleObjects(THREAD_SIZE,hClntThreadHandles,true,INFINITE);
	//关闭互斥句柄
	CloseHandle(mtx);
	return 0;
}

//用于错误信息打印
void ErrorHandler(char* message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
//处理客户端socket读写的线程函数
unsigned WINAPI ClntHandler(void* param)
{
	
	int i,strLen;
	char buf[BUF_SIZE];
	SOCKET clntSock=*((SOCKET*)param);
	while(1)
	{
		strLen = recv(clntSock,buf,BUF_SIZE,0);
		if(strLen<=0)
			break;
		send(clntSock,buf,strLen,0);
	}
	WaitForSingleObject(mtx,INFINITE);
	for(i=0;i<clntCnt;i++)
	{
		if(clntSocks[i]==clntSock)
		{
			while(i++<clntCnt-1)
				clntSocks[i]=clntSocks[i+1];
			break;
		}
		
	}
	clntCnt--;
	ReleaseMutex(mtx);
	closesocket(clntSock);
	return 0;
}


