#pragma once

#define WM_MYSOCKET WM_USER+1

#define ACCEPT 0
#define RECIEVE 1
#define CONNECT 2
#define SEND 3
#define CLOSE 4

#define BUF_SIZE 1024
// CMySocket ÃüÁîÄ¿±ê

class CMySocket : public CAsyncSocket
{
public:
	CMySocket();
	virtual ~CMySocket();

	CWnd* m_pWnd;

	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnSend(int nErrorCode);

};


