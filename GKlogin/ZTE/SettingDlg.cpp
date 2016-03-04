

/**************************************************************************************
			The Luzj's Zte Project
			//////////////////////
			Copyleft ? 2009 Luzj
		Author:Luzj		QQ:86829232
		http://blog.csdn.net/luzjqq
		Email: luzjcn@gmail.com
	///////////////////////////////////
关于Luzj's Zte认证端的声明：

1、本软件所有涉及的中兴认证的功能的实现均是通过黑盒分析得来，并未通过任何不正当方法获得。

2、本软件仅供研究学习之用，不得使用本软件损害中兴公司商业利益。

3、本软件不可用于任何商业和非法用途，否则责任自负。

4、本软件在发布前均通过一般性应用测试，但不保证任何情况下对机器无害，
由于未知的使用环境或不当的使用对计算机造成的损害，责任由使用者全部承担。

5.本软件版权没有，翻印不究，但请协助改进本作品。

6.本软件属开源软件，如果需要修改本软件源码以进行二次发布，也请继续公开源代码。

由于任何不遵守上叙条例引起的纠纷，均与本人无关，如不同意该声明请不要使用该软件，谢谢合作。
**************************************************************************************/
// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GKlogin.h"
#include "GKloginDlg.h"
#include "SettingDlg.h"
#include "AutoUpdate.h"
#include "ChangePWD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingDlg dialog


CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingDlg)
	DDX_Control(pDX, IDC_WEBURL, m_url);
	DDX_Control(pDX, IDC_AUTHOR_URL, m_author_url);
	DDX_Control(pDX, IDC_Modify_URL, m_modify_url);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CSettingDlg)
	ON_BN_CLICKED(IDC_CHK_WEB_AUTH, OnChkWebAuth)
	ON_BN_CLICKED(IDC_CHK_WEB_LOGOUT, OnChkWebLogout)
	ON_BN_CLICKED(IDC_CHK_ENABLE_WEBACCOUNT, OnChkEnableWebaccount)
	ON_BN_CLICKED(IDC_BTN_AUTO_UPDATE, OnBtnAutoUpdate)
	ON_BN_CLICKED(IDC_CHK_HTTP_HEART, OnChkHttpHeart)
	ON_BN_CLICKED(IDC_BTN_AUTO_WinPcapUPDATE, OnBtnAutoWinPcapUpdate)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHK_bTimingReauth, &CSettingDlg::OnBnClickedChkbtimingreauth)
	ON_BN_CLICKED(IDC_Advanced, &CSettingDlg::OnBnClickedAdvanced)
	ON_BN_CLICKED(IDC_AboutUS, &CSettingDlg::OnBnClickedAboutUS)
	ON_BN_CLICKED(IDC_ChangPWD_Button, &CSettingDlg::OnBnClickedChangpwdButton)
	ON_BN_CLICKED(IDC_WIFI_DOWN, &CSettingDlg::OnBnClickedWifiDown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingDlg message handlers

BOOL CSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CheckDlgButton(IDC_CHK_AUTOLOGON,Config.m_bAutologon?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_AUTORUN,Config.m_bAutorun?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_BUBBLE,Config.m_bShowBubble?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_WEB_AUTH,Config.m_bWebAuth?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_WEB_LOGOUT,Config.m_bWebLogout?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_ENABLE_WEBACCOUNT,Config.m_bEnableWebAccount?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_AUTO_UPDATE,Config.m_bAutoUpdate?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_DEBUG,Config.m_bDebug?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_AUTO_FILTER,Config.m_bAutoFilter?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_DHCP,Config.m_bDHCP?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_HTTP_HEART,Config.m_bHttpHeart?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_Ping, Config.m_Ping ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_Reauth, Config.m_Reauth ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_bTimingReauth, Config.m_bTimingReauth ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_UEIP, Config.m_UEIP ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_SlienceUp, Config.m_SlienceUp ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_ZTEAuth, Config.m_ZTEAuth ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_WebAuth, Config.m_WebAuth ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_WebAuth2, Config.m_WebAuth2 ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_CYClient, Config.m_CYClient ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_AutoBas, Config.m_AutoBas ? BST_CHECKED : BST_UNCHECKED);

	char szTemp[MAX_STRING];
	sprintf(szTemp,"%d",Config.m_iTimeout);
	GetDlgItem(IDC_TIMEOUT)->SetWindowText(szTemp);

	sprintf(szTemp,"%d",Config.m_iHeartInterval);
	GetDlgItem(IDC_HTTP_HEART_INTERVAL)->SetWindowText(szTemp);

	sprintf(szTemp, "%s", Config.m_csReauthTime);
	GetDlgItem(IDC_ReauthTime)->SetWindowText(szTemp);

	GetDlgItem(IDC_WEB_AUTH_URL)->SetWindowText(Config.m_csWebAuthUrl);
	GetDlgItem(IDC_WEB_LOGOUT_URL)->SetWindowText(Config.m_csWebLogoutUrl);
	GetDlgItem(IDC_WEB_USERNAME)->SetWindowText(Config.m_csWebUsername);
	GetDlgItem(IDC_WEB_PASSWORD)->SetWindowText(Config.m_csWebPassword);
	GetDlgItem(IDC_WEB_HEART_URL)->SetWindowText(Config.m_csHeartUrl);
	GetDlgItem(IDC_WEB_HEART_COOKIES)->SetWindowText(Config.m_csHeartCookies);
	GetDlgItem(IDC_ZTEServer)->SetWindowText(Config.m_csZTEServer);
	m_url.SetURL(STR_WEB_URL); m_author_url.SetURL(STR_AUTHOR_URL); 
	m_modify_url.SetURL(STR_Modify_URL); 
	GetDlgItem(IDC_VERSION)->SetWindowText(STR_AppName"  " STR_Version);

	OnChkWebAuth();
	OnChkWebLogout();
	OnChkEnableWebaccount();
	OnChkHttpHeart();
	OnBnClickedChkbtimingreauth();
	OnBnClickedAdvanced();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingDlg::OnOK()
{
	
#define BIND_BOOL_VAR_CONTROL(v, c) v=(IsDlgButtonChecked(c)==0)?false:true;
	BIND_BOOL_VAR_CONTROL(Config.m_bAutologon, IDC_CHK_AUTOLOGON);
	BIND_BOOL_VAR_CONTROL(Config.m_bWebAuth, IDC_CHK_WEB_AUTH);
	BIND_BOOL_VAR_CONTROL(Config.m_bAutorun, IDC_CHK_AUTORUN);
	BIND_BOOL_VAR_CONTROL(Config.m_bShowBubble, IDC_CHK_BUBBLE);
	BIND_BOOL_VAR_CONTROL(Config.m_bWebLogout, IDC_CHK_WEB_LOGOUT);
	BIND_BOOL_VAR_CONTROL(Config.m_bEnableWebAccount, IDC_CHK_ENABLE_WEBACCOUNT);
	BIND_BOOL_VAR_CONTROL(Config.m_bAutoUpdate, IDC_CHK_AUTO_UPDATE);
	BIND_BOOL_VAR_CONTROL(Config.m_bDebug, IDC_CHK_DEBUG);
	BIND_BOOL_VAR_CONTROL(Config.m_bAutoFilter, IDC_CHK_AUTO_FILTER);
	BIND_BOOL_VAR_CONTROL(Config.m_bDHCP, IDC_CHK_DHCP);
	BIND_BOOL_VAR_CONTROL(Config.m_bHttpHeart, IDC_CHK_HTTP_HEART);
	BIND_BOOL_VAR_CONTROL(Config.m_Ping, IDC_CHK_Ping);
	BIND_BOOL_VAR_CONTROL(Config.m_Reauth,IDC_CHK_Reauth);
	BIND_BOOL_VAR_CONTROL(Config.m_bTimingReauth,IDC_CHK_bTimingReauth);
	BIND_BOOL_VAR_CONTROL(Config.m_UEIP, IDC_UEIP);
	BIND_BOOL_VAR_CONTROL(Config.m_SlienceUp, IDC_CHK_SlienceUp);
	BIND_BOOL_VAR_CONTROL(Config.m_ZTEAuth ,IDC_CHK_ZTEAuth);
	BIND_BOOL_VAR_CONTROL(Config.m_WebAuth, IDC_CHK_WebAuth);
	BIND_BOOL_VAR_CONTROL(Config.m_WebAuth2, IDC_CHK_WebAuth2);
	BIND_BOOL_VAR_CONTROL(Config.m_CYClient, IDC_CHK_CYClient);
	BIND_BOOL_VAR_CONTROL(Config.m_AutoBas, IDC_CHK_AutoBas);

	char szTemp[MAX_STRING];
	GetDlgItem(IDC_TIMEOUT)->GetWindowText(szTemp,MAX_STRING);
	Config.m_iTimeout=atoi(szTemp);

	GetDlgItem(IDC_HTTP_HEART_INTERVAL)->GetWindowText(szTemp,MAX_STRING);
	Config.m_iHeartInterval = atoi(szTemp);

	GetDlgItem(IDC_ReauthTime)->GetWindowText(szTemp,MAX_STRING);
	Config.m_csReauthTime = szTemp;

#define BIND_STRING_VAR_CONTROL(v, c) {CString t; GetDlgItem(c)->GetWindowText(t); v = t;}

	BIND_STRING_VAR_CONTROL(Config.m_csWebAuthUrl, IDC_WEB_AUTH_URL);
	BIND_STRING_VAR_CONTROL(Config.m_csWebLogoutUrl, IDC_WEB_LOGOUT_URL);
	BIND_STRING_VAR_CONTROL(Config.m_csWebUsername, IDC_WEB_USERNAME);
	BIND_STRING_VAR_CONTROL(Config.m_csWebPassword, IDC_WEB_PASSWORD);
	BIND_STRING_VAR_CONTROL(Config.m_csHeartUrl, IDC_WEB_HEART_URL);
	BIND_STRING_VAR_CONTROL(Config.m_csHeartCookies, IDC_WEB_HEART_COOKIES);
	BIND_STRING_VAR_CONTROL(Config.m_csZTEServer, IDC_ZTEServer);
	Config.SaveConfig();
	CDialog::OnOK();
}

void CSettingDlg::OnChkWebAuth() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHK_WEB_AUTH))
	{
		GetDlgItem(IDC_WEB_AUTH_URL)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_WEB_AUTH_URL)->EnableWindow(FALSE);
	}
}

void CSettingDlg::OnChkWebLogout() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHK_WEB_LOGOUT))
	{
		GetDlgItem(IDC_WEB_LOGOUT_URL)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_WEB_LOGOUT_URL)->EnableWindow(FALSE);
	}
}

void CSettingDlg::OnChkEnableWebaccount() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHK_ENABLE_WEBACCOUNT))
	{
		GetDlgItem(IDC_WEB_USERNAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_WEB_PASSWORD)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_WEB_USERNAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_WEB_PASSWORD)->EnableWindow(FALSE);
	}
}
void CSettingDlg::OnBnClickedChkbtimingreauth()
{
	if (IsDlgButtonChecked(IDC_CHK_bTimingReauth))
	{
		GetDlgItem(IDC_ReauthTime)->EnableWindow(TRUE);
	}
	else{
		GetDlgItem(IDC_ReauthTime)->EnableWindow(FALSE);
	}
}
static int UpdateChoose;
static HANDLE m_ZTEUpdateThread;			//客户端升级守护线程,以便控制线程
static HANDLE m_WinPcapUpdateThread;		//WinPcap升级守护线程,以便控制线程

void CSettingDlg::OnBtnAutoUpdate() 
{
	// TODO: Add your control notification handler code here
	UpdateChoose = 1;
	CGKloginDlg *Dlg = (CGKloginDlg *)parent;
	if (m_ZTEUpdateThread == NULL)
	{
		AfxMessageBox("开始检查客户端更新，请稍后...", MB_TOPMOST);
		m_ZTEUpdateThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CheckUpdate, Dlg, 0, NULL);
	}
	else
		AfxMessageBox("正在检查客户端更新中，请稍后...", MB_TOPMOST);
}

void CSettingDlg::OnBtnAutoWinPcapUpdate()
{
	UpdateChoose = 2;
	CGKloginDlg *Dlg = (CGKloginDlg *)parent;
	if (m_WinPcapUpdateThread == NULL)
	{
		AfxMessageBox("开始检查WinPcap更新，请稍后...",  MB_TOPMOST);
		m_WinPcapUpdateThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CheckUpdate, Dlg, 0, NULL);
	}
	else
		AfxMessageBox("正在检查WinPcap更新中，请稍后..." , MB_TOPMOST);
}
DWORD CSettingDlg::CheckUpdate(void *para)
{
	CGKloginDlg *Dlg = (CGKloginDlg *)para;
	if (UpdateChoose==2){
		int ret;
		if (Dlg != NULL) {
			ret = Dlg->CheckWinPcapUpdate();
			if (ret == 2) {
				AfxMessageBox("WinPcap已经是最新版本的了！" , MB_TOPMOST);
				m_WinPcapUpdateThread = NULL;
				return 1;
			}
			if (ret == -1) {
				AfxMessageBox("远程服务器无法连接，请手动更新WinPcap！" , MB_TOPMOST);
				m_WinPcapUpdateThread = NULL;
				return 0;
			}

		}
	}
	else{
		int ret;
		if (Dlg != NULL) {
			ret = Dlg->CheckUpdate();
			if (ret == 2) {
				AfxMessageBox("客户端已经是最新版本的了！", MB_TOPMOST);
				m_ZTEUpdateThread = NULL;
				return 1;
			}
			if (ret == -1) {
				AfxMessageBox("获取本地信息失败或远程服务器无法连接，请手动更新！", MB_TOPMOST);
				m_ZTEUpdateThread = NULL;
				return 0;
			}
		}
	}
	return 1;
}
void CSettingDlg::OnChkHttpHeart() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHK_HTTP_HEART))
	{
		GetDlgItem(IDC_WEB_HEART_URL)->EnableWindow(TRUE);
		GetDlgItem(IDC_WEB_HEART_COOKIES)->EnableWindow(TRUE);
		GetDlgItem(IDC_HTTP_HEART_INTERVAL)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_WEB_HEART_URL)->EnableWindow(FALSE);
		GetDlgItem(IDC_WEB_HEART_COOKIES)->EnableWindow(FALSE);
		GetDlgItem(IDC_HTTP_HEART_INTERVAL)->EnableWindow(FALSE);
	}
}

void CSettingDlg::OnBnClickedAdvanced()
{
	RECT rc1 = { 0 }, rc2 = { 0 };
	if (!IsDlgButtonChecked(IDC_Advanced))
	{
		GetDlgItem(IDC_SettingSPLIT)->GetWindowRect(&rc1);
		GetWindowRect(&m_rc);
		rc2 = m_rc;
		rc2.bottom = rc1.top - 1;
		MoveWindow(&rc2, TRUE);
	}
	else
	{
		GetWindowRect(&rc1);
		m_rc.right += rc1.left - m_rc.left;
		m_rc.bottom += rc1.top - m_rc.top;
		m_rc.left = rc1.left;
		m_rc.top = rc1.top;
		MoveWindow(&m_rc, TRUE);
	}
	// TODO:  在此添加控件通知处理程序代码
}


void CSettingDlg::OnBnClickedAboutUS()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
	// TODO:  在此添加控件通知处理程序代码
}


void CSettingDlg::OnBnClickedChangpwdButton()
{
	CGKloginDlg *Dlg = (CGKloginDlg *)parent;
	if (Dlg->m_bAuth)
	{
		CChangePWD ChangePWDdlg;
		ChangePWDdlg.DoModal();
	}
	else
		AfxMessageBox("请登陆后再修改密码！", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
// TODO:  在此添加控件通知处理程序代码
}

static HANDLE WifiDownThread;			//Wifi程序下载守护线程,以便控制线程
DWORD CSettingDlg::WifiDownloads(void *para)
{
	CGKloginDlg *Dlg = (CGKloginDlg *)para;
	char szTempPath[MAX_PATH], szTempName[MAX_PATH];
	GetTempPath(MAX_PATH, szTempPath);
	sprintf(szTempName, "%sLiebao.exe", szTempPath);

	if (DownLoadFile(szTempName, LAN_URL "/Liebao.exe", NULL, DOWNTIMEOUT, NULL, NULL) != 0)
	{
		if (DownLoadFile(szTempName, WEB_URL "/Liebao.exe", NULL, DOWNTIMEOUT+60, NULL, NULL) != 0)
		{
			AfxMessageBox(_T("下载猎豹Wifi失败，请重试！"), MB_TOPMOST);
			WifiDownThread = NULL;
			return -1;
		}
	}
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.lpFile = szTempName;
	ShExecInfo.nShow = SW_SHOW;
	if (!ShellExecuteEx(&ShExecInfo)) {
		AfxMessageBox(_T("启动猎豹Wifi安装程序失败！"), MB_TOPMOST);
		WifiDownThread = NULL;
		return -2;
	}
	else{
		WifiDownThread = NULL;
	}
	return 0;
}

void CSettingDlg::OnBnClickedWifiDown()
{
	CGKloginDlg *Dlg = (CGKloginDlg *)parent;
	if (Dlg->m_bAuth)
	{
		if (WifiDownThread == NULL)
			WifiDownThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)WifiDownloads, Dlg, 0, NULL);
		else
			AfxMessageBox("程序正在下载中，请稍后...", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}
	else
		AfxMessageBox("请登陆后再下载程序！", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	// TODO:  在此添加控件通知处理程序代码
}
