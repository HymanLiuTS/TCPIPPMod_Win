
// ServDlg.h : 头文件
//

#pragma once

#include "MySocket.h"


// CServDlg 对话框
class CServDlg : public CDialogEx
{
// 构造
public:
	CServDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERV_DIALOG };

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
	CListBox m_ctListBox;
private:
	CMySocket m_Sock;
	CList<CMySocket*> m_pClnts;
protected:
	afx_msg LRESULT OnMysocket(WPARAM wParam, LPARAM lParam);
};
