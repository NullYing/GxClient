
// GKloginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GKlogin.h"
#include "GKloginDlg.h"
#include "afxdialogex.h"
#include "SettingDlg.h"
#include "afxwin.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int dpiX;
int dpiY;

CComplaint::CComplaint(CWnd* pParent /*=NULL*/)
	: CDialogEx(CComplaint::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CComplaint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CComplaint, CDialog)
	ON_STN_CLICKED(IDC_GXerweima, &CComplaint::OnStnClickedGxerweima)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OEM_EDIT, m_AboutUs);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHK_Restore, &CAboutDlg::OnBnClickedChkRestore)
END_MESSAGE_MAP()


// CGKloginDlg 对话框



CGKloginDlg::CGKloginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGKloginDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//认证状态为 未认证
	m_bAuth = FALSE;
}

void CGKloginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_USERNAME, _T(m_ccb_username));
	DDX_Control(pDX, IDC_NETCARD, _T(m_ccbNetCard));
	DDX_Control(pDX, IDC_Banner, Banner_Var);
}

BEGIN_MESSAGE_MAP(CGKloginDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(MENU_SHOW, OnTrayShow)
	ON_MESSAGE(WM_USER_TRAY_NOTIFICATION, OnTrayNotification)
	ON_WM_TIMER()
	//按钮
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_LOGOFF, OnLogoff)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(MENU_EXIT, OnExit)
	ON_BN_CLICKED(IDC_SETTING, OnSetting)
	ON_BN_CLICKED(IDC_LOGSHOW, OnLogshow)
	ON_BN_CLICKED(IDC_Feedback, OnBnClickedFeedback)
	ON_STN_CLICKED(IDC_Banner, OnStnClickedBanner)

	ON_CBN_SELCHANGE(IDC_USERNAME, OnSelchangeUsername)
	ON_COMMAND(IDOK, &CGKloginDlg::OnIdok)
END_MESSAGE_MAP()


// CGKloginDlg 消息处理程序
BOOL CGKloginDlg::OnInitDialog()
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	m_hIcon = AfxGetApp()->LoadIcon(IDI_SmallICON);
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	//创建托盘图标
	m_tray.Create(this, WM_USER_TRAY_NOTIFICATION, STR_AppName, m_hIcon, 0);

	CDC* dc = GetDC();
	int dpiX = GetDeviceCaps(dc->GetSafeHdc(), LOGPIXELSX);
	int dpiY = GetDeviceCaps(dc->GetSafeHdc(), LOGPIXELSY);
	SetDPI(dpiX, dpiY);
	ReleaseDC(dc);
	// TODO:  在此添加额外的初始化代码
	//读取配置到文件配置对象中去
	
	char szTempPath[MAX_PATH], szTempName[MAX_PATH];
	GetTempPath(MAX_PATH, szTempPath);

	sprintf(szTempName, "%sBanner.png", szTempPath);
	if ((_access(szTempName, 0)) != -1)
	{
		HWND hwnd = GetSafeHwnd(); //获取窗口的HWND
		::InvalidateRect(hwnd, NULL, true); //或者 ::InvalidateRect( hwnd, NULL, false );
		::UpdateWindow(hwnd);
		//若使用前不想把原来绘制的图片去掉，可以删去上面那三段
		CDC *pDC = GetDC();
		CImage Image;
		Image.Load(szTempName);
		if (Image.IsNull())
		{
			//Log(I_MSG, "Banner加载失败！");
			DeleteFile(szTempName);
		}
		if (BannerPicture != NULL)
		DeleteObject(BannerPicture);
		CStatic* pWnd = (CStatic*)GetDlgItem(IDC_Banner);
		BannerPicture = Image.Detach();
		pWnd->SetBitmap(BannerPicture);
		int width = 377, height = 80;
		pWnd->SetWindowPos(NULL, 0, 0, (size_t)(width / DEFAULT_DPI*dpiX), (size_t)(height / DEFAULT_DPI*dpiY), SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		//pWnd->SetWindowPos(NULL,0,0,377,80,SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		//Image.Draw(pDC->m_hDC, 230, 110);
		Image.Destroy();
		ReleaseDC(pDC);
	}
	else{
		CStatic* pWnd = (CStatic*)GetDlgItem(IDC_Banner); // 得到 Picture Control 句柄  
		CImage image;
		LoadImageFromResource(&image, IDB_Banner, _T("PNG"));
		BannerPicture = image.Detach();
		pWnd->SetBitmap(BannerPicture);
		int width = 377, height = 80;
		pWnd->SetWindowPos(NULL, 0, 0, (size_t)(width / DEFAULT_DPI*dpiX), (size_t)(height / DEFAULT_DPI*dpiY), SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		//pWnd->SetWindowPos(NULL,0,0,377,80,SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	}
	
	initialize();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGKloginDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == SC_MINIMIZE || (nID & 0xFFF0) == SC_CLOSE)
	{
		ShowWindow(SW_HIDE);
		//SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
		SetBubble("提示", STR_AppName" 正在后台运行着……", 1);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGKloginDlg::OnPaint()
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
HCURSOR CGKloginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGKloginDlg::OnTrayShow()
{
	if (IsWindowVisible())
	{
		ShowWindow(SW_HIDE);
		//SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
	}
	else
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(SW_SHOWNA);
		BringWindowToTop();
	}
}

LONG CGKloginDlg::OnTrayNotification(WPARAM wparam, LPARAM lparam)
{
	CMenu	m_menu;
	CPoint	m_point;
	switch (lparam)
	{
	case WM_RBUTTONDOWN:
		m_menu.CreatePopupMenu();
		if (IsWindowVisible())
		{
			m_menu.AppendMenu(MF_ENABLED, MENU_SHOW, _T("隐藏窗口"));
		}
		else
		{
			m_menu.AppendMenu(MF_ENABLED, MENU_SHOW, _T("显示窗口"));
		}
		m_menu.AppendMenu(MF_ENABLED, MF_SEPARATOR);
		if (!m_bAuth)
			m_menu.AppendMenu(MF_ENABLED, IDC_START, _T("登陆认证"));
		else
			m_menu.AppendMenu(MF_ENABLED, IDC_LOGOFF, _T("退出认证"));
		m_menu.AppendMenu(MF_ENABLED, MF_SEPARATOR);
		m_menu.AppendMenu(MF_ENABLED, MENU_EXIT, _T("退 出"));
		GetCursorPos(&m_point);
		m_menu.TrackPopupMenu(TPM_LEFTALIGN, m_point.x, m_point.y, this);
		break;
	case WM_LBUTTONDOWN:
		if (IsWindowVisible())
		{
			ShowWindow(SW_HIDE);
			//SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
		}
		else
		{
			SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			ShowWindow(SW_SHOWNA);
			BringWindowToTop();
		}
		break;
	}
	return 0;
}
void CGKloginDlg::OnBnClickedFeedback()
{
	
	// TODO:  在此添加控件通知处理程序代码
	CComplaint dlg;
	dlg.parent = this;
	dlg.DoModal();
}
void CGKloginDlg::OnSetting()
{
	CSettingDlg dlg;
	dlg.parent = this;
	//dlg.SetParent(this);	
	dlg.DoModal();
}
BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_SmallICON);
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	GetDlgItem(IDC_NameVersion)->SetWindowText(_T(STR_AppName "  版本号：" STR_Version));
	CEdit *editshow;
	editshow = (CEdit*)GetDlgItem(IDC_OEM_EDIT);
	int nLength = editshow->SendMessage(WM_GETTEXTLENGTH);
	editshow->SetSel(nLength, nLength);
	editshow->ReplaceSel("      感谢yzx老师开放了莞香客户端的源代码，请原谅我擅自把名字改为莞香客户端，这个版本主要是对之前校园网客户端程序的优化和美化，尽量的避免因人为操作导致的各种无法认证问题！");
	editshow->ReplaceSel("\r\n寄语：\r\n      每个人在成长过程中都难免犯一些错误，我们也一样，软件的故障也是我们不希望看到的。尽管我们不能做到让每个用户满意，但是我们接受各种建议和批评！我们因兴趣而聚集在一起，为了各自的梦想而努力。希望我们的工作能得到大家的支持，这就是给予我们最大的回报！");
	CEdit *editshow2;
	editshow2 = (CEdit*)GetDlgItem(IDC_OEM_EDIT2);
	int nLength2 = editshow2->SendMessage(WM_GETTEXTLENGTH);
	editshow2->SetSel(nLength2, nLength2);
	editshow2->ReplaceSel("莞香广科是学生自建的互联网创新团队。团队成立于2012年5月，以校园论坛起步，逐渐发展了校园网资源共享站、媒体中心、微信助手等项目。经过三年多的发展，莞香广科已经成为校内知名的媒体组织与学生平台。现团队成员都是广科的在校生，毕业的团队成员其中有产品运营、媒体编辑、程序员、设计者、摄影师等。");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAboutDlg::OnBnClickedChkRestore()
{
	int result = MessageBox(_T("本项操作会清除所有账号密码信息！请确认是否恢复默认设置？"), _T("警告"), MB_ICONQUESTION | MB_YESNO);
	switch (result)//注意！使用Unicode应用TEXT包围字串
	{
	case IDYES:
	{
		char pszFullPath[MAX_STRING];
		char pszFilename[MAX_STRING] = CONFIGNAME;

		Config.GetFullPathToFile(pszFullPath, pszFilename);

		DeleteFile(pszFullPath);

		PostMessage(WM_QUIT);
		Sleep(800);
		//获取exe程序当前路径  
		TCHAR szAppName[MAX_PATH];
		::GetModuleFileName(theApp.m_hInstance, szAppName, MAX_PATH);
		CString strAppFullName;
		strAppFullName.Format(_T("%s"), szAppName);
		//重启程序  
		STARTUPINFO StartInfo;
		PROCESS_INFORMATION procStruct;
		memset(&StartInfo, 0, sizeof(STARTUPINFO));
		StartInfo.cb = sizeof(STARTUPINFO);
		::CreateProcess(
			(LPCTSTR)strAppFullName,
			NULL,
			NULL,
			NULL,
			FALSE,
			NORMAL_PRIORITY_CLASS,
			NULL,
			NULL,
			&StartInfo,
			&procStruct);
	}
	case IDNO:
		return;
	}

}
BOOL LoadImageFromResource(CImage *pImage, UINT nResID, LPCTSTR lpTyp)
{
	if (pImage == NULL)
		return false;
	pImage->Destroy();
	// 查找资源
	HRSRC hRsrc = ::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), lpTyp);
	if (hRsrc == NULL)
		return false;
	// 加载资源
	HGLOBAL hImgData = ::LoadResource(AfxGetResourceHandle(), hRsrc);
	if (hImgData == NULL)
	{
		::FreeResource(hImgData);
		return false;
	}
	// 锁定内存中的指定资源
	LPVOID lpVoid = ::LockResource(hImgData);
	LPSTREAM pStream = NULL;
	DWORD dwSize = ::SizeofResource(AfxGetResourceHandle(), hRsrc);
	HGLOBAL hNew = ::GlobalAlloc(GHND, dwSize);
	LPBYTE lpByte = (LPBYTE)::GlobalLock(hNew);
	::memcpy(lpByte, lpVoid, dwSize);
	// 解除内存中的指定资源
	::GlobalUnlock(hNew);
	// 从指定内存创建流对象
	HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
	if (ht != S_OK)
	{
		GlobalFree(hNew);
	}
	else
	{
		// 加载图片
		pImage->Load(pStream);
		GlobalFree(hNew);
	}
	// 释放资源
	::FreeResource(hImgData);
	return true;
}

BOOL CComplaint::OnInitDialog()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_SmallICON);
	this->SetIcon(m_hIcon, FALSE);		// 设置小图标
	
	CStatic* pWnd = (CStatic*)GetDlgItem(IDC_GXerweima); // 得到 Picture Control 句柄  
	CImage image;
	LoadImageFromResource(&image, IDB_erweima, _T("PNG"));
	HBITMAP hBmp = image.Detach();
	pWnd->SetBitmap(hBmp);
	int width = 240, height = 240;
	pWnd->SetWindowPos(NULL, 0, 0, (size_t)(width / DEFAULT_DPI*dpiX), (size_t)(height / DEFAULT_DPI*dpiY), SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	fontTitle.CreatePointFont(150, "微软雅黑");
	ShowMsg(FALSE);
	return TRUE;
}



void CComplaint::OnStnClickedGxerweima()
{
	// TODO:  在此添加控件通知处理程序代码
	CHyperLink Link;
	Link.GotoURL(URL_Help, SW_SHOW);
	//::ShellExecute(this->GetSafeHwnd(), _T("open"), URL_Help, NULL, NULL, SW_SHOWNORMAL);

}
HBRUSH CComplaint::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性   

	switch (pWnd->GetDlgCtrlID())
	{
	case IDC_STATICBig1:
		//pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SelectObject(&fontTitle);
		break;
	case IDC_STATICBig2:
		//pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SelectObject(&fontTitle);
		break;
	case IDC_STATICBig3:
		//pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SelectObject(&fontTitle);
		break;
		// TODO:  如果默认的不是所需画笔，则返回另一个画笔   	
	}
	return hbr;
}



void moveWindow(CWnd &wnd)
{
	CRect rect;
	wnd.GetWindowRect(rect);
	wnd.GetParent()->ScreenToClient(rect);
	rect.left = (int)(rect.left*DEFAULT_DPI / dpiX);
	rect.right = (int)(rect.right*DEFAULT_DPI / dpiX);
	rect.top = (int)(rect.top*DEFAULT_DPI / dpiY);
	rect.bottom = (int)(rect.bottom*DEFAULT_DPI / dpiY);
	rect.OffsetRect(CPoint((int)((dpiX - DEFAULT_DPI) / 2), (int)((dpiY - DEFAULT_DPI) / 2)));

	wnd.MoveWindow(rect);
}

void SetDPI(int x, int y)
{
	dpiX = x;
	dpiY = y;
}

double GetDPIX(double X)
{
	X = X / DEFAULT_DPI * dpiX;
	return X;
}

double GetDPIY(double Y)
{
	Y = Y / DEFAULT_DPI * dpiY;
	return Y;
}

