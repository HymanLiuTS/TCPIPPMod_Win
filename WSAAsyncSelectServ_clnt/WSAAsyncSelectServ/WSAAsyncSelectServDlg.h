
// WSAAsyncSelectServDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 30
#define WM_SOCKET WM_USER+1


// CWSAAsyncSelectServDlg 对话框
class CWSAAsyncSelectServDlg : public CDialogEx
{
// 构造
public:
	CWSAAsyncSelectServDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WSAASYNCSELECTSERV_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_list;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
private:
	SOCKET m_servSock;
	SOCKET m_clntSock;
	SOCKADDR_IN m_servAddr;
	SOCKADDR_IN m_clntAddr;
	int m_strLen;
	int m_clntAddrSz;

	char buf[BUF_SIZE];

	CMap<SOCKET,SOCKET,SOCKADDR_IN,SOCKADDR_IN> m_sockMap;

	WSAData m_wsaData;

	void PrintClientConnectMsg(void);
	void PrintClientSendMsg(SOCKET sock);
};
