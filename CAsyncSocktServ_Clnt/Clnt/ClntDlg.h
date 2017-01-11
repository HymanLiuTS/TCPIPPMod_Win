
// ClntDlg.h : 头文件
//

#pragma once
#include "../Serv/MySocket.h"
#include "afxwin.h"

// CClntDlg 对话框
class CClntDlg : public CDialogEx
{
// 构造
public:
	CClntDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CLNT_DIALOG };

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
	afx_msg void OnBnClickedButton1();
	CMySocket m_servSock;
	CEdit m_send;
	CEdit m_recv;
protected:
	afx_msg LRESULT OnMysocket(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButton2();
};
