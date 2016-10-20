#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>

#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 2048
#define BUF_SMALL 100

void ErrorHandling(char*);
void SendErrorMessage(SOCKET);
void SendData(SOCKET sock,char* ct,char* filename);
unsigned WINAPI RequestHandler(void *arg);
char* ContentType(char* file);


int main(int argc,char *argv[])
{
	WSADATA wsaData;
	SOCKET hServSock,hClntSock;
	SOCKADDR_IN servAddr,clntAddr;

	HANDLE hThread;
	DWORD dwThreadID;
	int clntAdrSize;

	if(argc!=2)
	{
		printf("Usage %s <port>\n",argv[0]);
		exit(1);
	}

	if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
			ErrorHandling("WSAStartup error");

	hServSock=socket(AF_INET,SOCK_STREAM,0);
	memset(&servAddr,0,sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_port=htons(atoi(argv[1]));

	if(bind(hServSock,(SOCKADDR*)&servAddr,sizeof(servAddr))==SOCKET_ERROR)
		ErrorHandling("bind error");

	if(listen(hServSock,5)==SOCKET_ERROR)
		ErrorHandling("listen error");

	while(1)
	{
		clntAdrSize=sizeof(clntAddr);
		hClntSock=accept(hServSock,(SOCKADDR*)&clntAddr,&clntAdrSize);
		printf("Connection Request :%s:%d\n",
			inet_ntoa(clntAddr.sin_addr),ntohs(clntAddr.sin_port));

		hThread=(HANDLE)_beginthreadex(
			NULL,
			0,
			RequestHandler,
			(void*)hClntSock,
			0,
			(unsigned*)&dwThreadID);
	}
	closesocket(hServSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI RequestHandler(void *arg)
{
	SOCKET hClntSock=(SOCKET)arg;
	char buf[BUF_SIZE];
	char method[BUF_SMALL];
	char ct[BUF_SMALL];
	char fileName[BUF_SMALL];
	int flag;

	recv(hClntSock,buf,BUF_SIZE,0);
	if(strstr(buf,"HTTP/")==NULL)
	{
		SendErrorMessage(hClntSock);
		closesocket(hClntSock);
		return 1;	
	}
	strcpy(method,strtok(buf," /"));
	flag=strcmp(method,"GET");
	if(flag!=0)//查看是否为GET方式请求
		SendErrorMessage(hClntSock);

	strcpy(fileName,strtok(NULL," /"));
	strcpy(ct,ContentType(fileName));
	SendData(hClntSock,ct,fileName);
}
void SendData(SOCKET sock,char* ct,char* filename)
{
	char protocol[]="HTTP/1.0 200 OK\r\n";
	char servName[]="Server:simple web server\r\n";
	char cntlen[]="Content-length:2048\r\n";
	char cntType[BUF_SMALL];
	char buf[BUF_SIZE]="Hello World";
	FILE* sendFile;

	sprintf(cntType,"Content-type:%s\r\n\r\n",ct);

	sendFile=fopen(filename,"r");
	if(sendFile==NULL)
	{
		SendErrorMessage(sock);
		return;
	}
	/*传输头信息*/
	send(sock,protocol,strlen(protocol),0);
	send(sock,servName,strlen(servName),0);
	send(sock,cntlen,strlen(cntlen),0);
	send(sock,cntType,strlen(cntType),0);

	/*传输请求数据*/
	while(fgets(buf,BUF_SIZE,sendFile)!=NULL)
		send(sock,buf,strlen(buf),0);

	closesocket(sock);
}

void SendErrorMessage(SOCKET sock)
{
	char protocol[]="HTTP/1.0 400 Bad Request\r\n";
	char servName[]="Server:simple web server\r\n";
	char cntlen[]="Content-length:2048\r\n";
	char cntType[]="Content-type:text/html\r\n\r\n";
	char content[]="<html><head><title>NETWORK</title></head>"
		"<body><font size=+5><br>发生错误！查看请求文件名和请求方式！"
		"</font></body></html>";
	send(sock,protocol,strlen(protocol),0);
	send(sock,servName,strlen(servName),0);
	send(sock,cntlen,strlen(cntlen),0);
	send(sock,cntType,strlen(cntType),0);
	send(sock,content,strlen(content),0);
	closesocket(sock);
}

char* ContentType(char* file)
{
	char extension[BUF_SMALL];
	char filename[BUF_SMALL];
	strcpy(filename,file);
	strtok(filename,".");
	strcpy(extension,strtok(NULL,"."));
	if(!strcmp(extension,"html")||!strcmp(extension,"html"))
		return "text/html";
	else
		return "text/plain";
}

void ErrorHandling(char* message)
{
	fputs(message,stderr);
	fputc('\c',stderr);
	exit(1);
}
