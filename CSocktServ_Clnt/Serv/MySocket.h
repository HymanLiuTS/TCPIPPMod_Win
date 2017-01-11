#pragma once

#define WM_MYSOCKET WM_USER+1

#define ACCEPT 0
#define RECIEVE 1
#define CLOSE 2

#define BUF_SIZE 1024
// CMySocket ÃüÁîÄ¿±ê

class CMySocket : public CSocket
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


