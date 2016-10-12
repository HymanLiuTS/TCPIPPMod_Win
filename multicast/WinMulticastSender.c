#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include <ws2ipdef.h>

#pragma comment(lib,"ws2_32.lib")

#define BUFF_SIZE 1024
void ErrorHandling(const char* message);

int main(int argc,char* argv[])
{
	WSADATA wsaData;
	SOCKET sock;
	SOCKADDR_IN addr;
	FILE* fp;
	int strLen;
	char message[BUFF_SIZE];
	int ttl=64;

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
	addr.sin_addr.s_addr=inet_addr("224.0.0.8");
	addr.sin_port=htons(atoi("8888"));

	setsockopt(sock,IPPROTO_IP,IP_MULTICAST_TTL,(char*)&ttl,sizeof(ttl));
	//fread file
	fp=fopen("news.txt","r+");
	if(fp==NULL)
	{
		closesocket(sock);
	}
	//sendto message
	while(!feof(fp))
	{
		strLen=fread(message,sizeof(char),BUFF_SIZE,fp);
		sendto(sock,message,strLen,0,(SOCKADDR*)&addr,sizeof(addr));
	}
	

	//close socket
	fclose(fp);
	closesocket(sock);
	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}