
// ClntDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Clnt.h"
#include "ClntDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
//	afx_msg LRESULT OnMysocket(WPARAM wParam, LPARAM lParam);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_MESSAGE(WM_MYSOCKET, &CAboutDlg::OnMysocket)
END_MESSAGE_MAP()


// CClntDlg 对话框




CClntDlg::CClntDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClntDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClntDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_send);
	DDX_Control(pDX, IDC_EDIT2, m_recv);
}

BEGIN_MESSAGE_MAP(CClntDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CClntDlg::OnBnClickedButton1)
	ON_MESSAGE(WM_MYSOCKET, &CClntDlg::OnMysocket)
	ON_BN_CLICKED(IDC_BUTTON2, &CClntDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CClntDlg 消息处理程序

BOOL CClntDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	WSAData wsaData;
	AfxSocketInit(&wsaData);

	
	
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CClntDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClntDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CClntDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CClntDlg::OnBnClickedButton1()
{
	char buf[BUF_SIZE];
	int strLen=m_send.GetWindowText(buf,BUF_SIZE);
	m_servSock.Send(buf,strLen,0);
}



afx_msg LRESULT CClntDlg::OnMysocket(WPARAM wParam, LPARAM lParam)
{
	CMySocket* pSock=(CMySocket*)wParam;
	int param=*((int*)(lParam));
	int recvLen;
	char buf[BUF_SIZE];
	switch(param)
	{
	case RECIEVE:
		{
			int strLen = pSock->Receive(buf,BUF_SIZE-1,0);
			buf[strLen]=0;
			CString str;
			str.Format("%s",buf);
			m_recv.SetWindowText(str);
		}
		break;
	default:
		break;
	}
	return 0;
}



void CClntDlg::OnBnClickedButton2()
{
	m_servSock.Create();
	SOCKADDR_IN servAddr;
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	servAddr.sin_port=htons(8888);
	m_servSock.m_pWnd=this;
	BOOL ret = m_servSock.Connect((SOCKADDR*)&servAddr,sizeof(servAddr));
}
