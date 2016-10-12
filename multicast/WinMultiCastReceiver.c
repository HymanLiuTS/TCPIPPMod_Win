#include<stdio.h>
#include<stdlib.h>
#include<WinSock2.h>
#include <ws2ipdef.h>
#define BUFF_SIZE 1024
#pragma comment(lib,"ws2_32.lib")

void ErrorHandling(const char* message);

int main(int argc,char* argv[])
{
	WSADATA wsaData;
	SOCKET sock;
	SOCKADDR_IN addr;
	char message[BUFF_SIZE];
	int strLen;
	struct ip_mreq mreq;

	if(argc!=3)
	{
		printf("Usage %s <Multicast address>  <port>\n",argv[0]);
		//exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2),&wsaData)==SOCKET_ERROR)
		ErrorHandling("WSAStartup() error");

	//create socket
	sock=socket(AF_INET,SOCK_DGRAM,0);
	if(sock==INVALID_SOCKET)
		ErrorHandling("socket() error");

	

	//prepare address
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(atoi("8888"));

	//bind
	if(bind(sock,(SOCKADDR*)&addr,sizeof(addr))==SOCKET_ERROR)
		ErrorHandling("bind() error");

	//set multicast socket option
	mreq.imr_multiaddr.s_addr=inet_addr("224.0.0.8");
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);
	setsockopt(sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,(const char*)&mreq,sizeof(mreq));

	//recieve message
	while(1)
	{
		strLen = recvfrom(sock,message,BUFF_SIZE-1,0,NULL,0);
		if(strLen<0)
			break;
		message[strLen]=0;
		fputs(message,stdout);
	}
	//close socket
	closesocket(sock);
	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}