
// WSAAsyncSelectServDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WSAAsyncSelectServ.h"
#include "WSAAsyncSelectServDlg.h"
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
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWSAAsyncSelectServDlg 对话框




CWSAAsyncSelectServDlg::CWSAAsyncSelectServDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWSAAsyncSelectServDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWSAAsyncSelectServDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_list);
}

BEGIN_MESSAGE_MAP(CWSAAsyncSelectServDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CWSAAsyncSelectServDlg 消息处理程序

BOOL CWSAAsyncSelectServDlg::OnInitDialog()
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
	WSAStartup(MAKEWORD(2,2),&m_wsaData);
	m_servSock=socket(AF_INET,SOCK_STREAM,0);
	
	memset(&m_servAddr,0,sizeof(m_servAddr));
	m_servAddr.sin_family=AF_INET;
	m_servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	m_servAddr.sin_port=htons(atoi("8888"));

	bind(m_servSock,(SOCKADDR*)&m_servAddr,sizeof(m_servAddr));

	listen(m_servSock,5);

	WSAAsyncSelect(m_servSock,this->m_hWnd,WM_SOCKET,FD_ACCEPT|FD_READ);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWSAAsyncSelectServDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWSAAsyncSelectServDlg::OnPaint()
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
HCURSOR CWSAAsyncSelectServDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CWSAAsyncSelectServDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch(message)
	{
		case WM_SOCKET:
			{
				SOCKET sock = (SOCKET) wParam;
				if(m_servSock == sock)
				{
					//获取
					m_clntAddrSz=sizeof(m_clntAddr);
					m_clntSock=accept(m_servSock,(SOCKADDR*)&m_clntAddr,&m_clntAddrSz);
					WSAAsyncSelect(m_clntSock,this->m_hWnd,WM_SOCKET,FD_READ);
					//将m_clntSock和m_clntAddr存到映射中
					m_sockMap.SetAt(m_clntSock,m_clntAddr);
					PrintClientConnectMsg();
					
				}
				else
				{
					m_strLen = recv(sock,buf,BUF_SIZE - 1,0);
					send(sock,buf,m_strLen,0);
					buf[m_strLen]=0;
					PrintClientSendMsg(sock);
				}
			}
			break;
		default:
			break;
	}

	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}



void CWSAAsyncSelectServDlg::PrintClientConnectMsg(void)
{
	CString str;
	CTime locTime=CTime::GetCurrentTime();    //获取当前时间
	CString strTime=locTime.Format("%Y-%m-%d %H:%M:%S");//将当前时间转换成CString类型
	str.Format("%s Client %s:%d Connectted",strTime,inet_ntoa(m_clntAddr.sin_addr),m_clntAddr.sin_port);
	this->m_list.AddString(str);
}


void CWSAAsyncSelectServDlg::PrintClientSendMsg(SOCKET sock)
{
	CString str;
	m_clntAddr=m_sockMap[sock];
	CTime locTime=CTime::GetCurrentTime();   
	CString strTime=locTime.Format("%Y-%m-%d %H:%M:%S");//将当前时间转换成CString类型
	str.Format("%s Client %s:%d say:%s",strTime,inet_ntoa(m_clntAddr.sin_addr),m_clntAddr.sin_port,buf);
	this->m_list.AddString(str);
}
