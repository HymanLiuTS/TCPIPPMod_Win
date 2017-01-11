// MySocket.cpp : 实现文件
//

#include "stdafx.h"
#include "Serv.h"
#include "MySocket.h"




// CMySocket

CMySocket::CMySocket()
{
}

CMySocket::~CMySocket()
{
}


// CMySocket 成员函数


void CMySocket::OnAccept(int nErrorCode)
{
	int param=ACCEPT;
	if(m_pWnd!=NULL)
		m_pWnd->SendMessage(WM_MYSOCKET,(WPARAM)this,(LPARAM)&param);
	CSocket::OnAccept(nErrorCode);
}


void CMySocket::OnReceive(int nErrorCode)
{
	int param=RECIEVE;
	if(m_pWnd!=NULL)
		m_pWnd->SendMessage(WM_MYSOCKET,(WPARAM)this,(LPARAM)&param);
	CSocket::OnReceive(nErrorCode);
}


void CMySocket::OnClose(int nErrorCode)
{
	int param=CLOSE;
	if(m_pWnd!=NULL)
		m_pWnd->SendMessage(WM_MYSOCKET,(WPARAM)this,(LPARAM)&param);
	CSocket::OnClose(nErrorCode);
}


void CMySocket::OnConnect(int nErrorCode)
{
	CSocket::OnConnect(nErrorCode);
}


void CMySocket::OnSend(int nErrorCode)
{
	CSocket::OnSend(nErrorCode);
}

