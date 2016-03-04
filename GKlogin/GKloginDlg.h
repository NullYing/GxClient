
// GKloginDlg.h : 头文件
//

#pragma once
#include "Define.h"
#include "Resource.h"
#include "SystemTray.h"
#include "PingThread.h"
#include "Config.h"
#include "afxwin.h"
#define WM_USER_TRAY_NOTIFICATION (WM_USER+0x101)//托盘
#define DEFAULT_DPI 96.0

extern CConfig Config;

// CGKloginDlg 对话框
class CGKloginDlg : public CDialogEx
{
// 构造
public:
	CGKloginDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GKLOGIN_DIALOG };
	CComboBox	m_ccb_username;
	CComboBox	m_ccbNetCard;

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
protected:
	//界面
	afx_msg LONG OnTrayNotification(WPARAM wparam, LPARAM lparam);  //托盘菜单
	RECT			m_rc;					//存放着窗体的矩形区域
	CEdit	*editLog;
	bool logshowstatus;
	afx_msg void OnSelchangeUsername();
	void Complaint();
	HBITMAP BannerPicture;

	CSystemTray		m_tray;					//系统托盘图标

	char *	GetOSVersion();
	afx_msg void OnTimer(UINT nIDEvent);
	time_t			m_startTime;			//认证的开始时间

	void initialize();   //中兴认证初始化函数
	//网卡信息
	char *	GetAdapterInfo(const char *descript);

	static char *	ToNPFName(const char *GUID);
	static char *	ToTCPName(const char *GUID);
	DWORD WINAPI IpconfigRenew();
	int TestAdapter(const char *name);
	CStringArray	 m_csAdapters;			//网卡列表
	pcap_t			* m_adapterHandle;		//适配器句柄

	


	//服务控制
	bool			ShareServiceStatus = 0; //ICS服务标志 0为将不启动ICS服务，1为将启动ICS服务
	afx_msg DWORD StopService(LPCTSTR pszName);
	afx_msg DWORD StartServiceA(LPCTSTR pszName);
	afx_msg DWORD DetectServiceStats(LPCTSTR pszName);
	afx_msg int DHCP_Auto();
	static bool EnableDHCP(const char* wcAdapterName, const bool enable);
	int WebPop();

	//提醒模块
	void	SetBubble(char * title, char * content, int timeout = 1000);


	int Notice(int id);
	
	void WebFaultMsg(const char *msg);




	//按钮
	afx_msg void OnStart();
	afx_msg void OnLogoff();
	afx_msg void OnExit();
	afx_msg void OnLogshow();
	afx_msg void OnSetting();
	afx_msg void OnTrayShow();
	afx_msg void OnBnClickedFeedback();

	//网页认证	
	char*	HttpAuth(BOOL bForce);
	char *  HttpAuthOut(BOOL bForce);
	void	StartWebAuth();

	//网页心跳
	afx_msg void OnHttpHeart();

	//Banner
	CStatic Banner_Var;
	afx_msg void OnStnClickedBanner();
	afx_msg void Banner_Show();

	//信息获取
	void GetAccountMsg();
	int HavingConnect();

	//流量
	long    tv_sec;         /* seconds */
	long    tv_usec;        /* and microseconds */
	float	Flux;			//流量
	static unsigned WINAPI __stdcall NetFlux(void *para);
	static void dispatcher_handler(u_char *, const struct pcap_pkthdr *, const u_char *);
public:
	//界面
	CStatusBarCtrl  m_StatusBar;			//状态栏

	bool			m_bAuth;				//是否已经认证
	int CheckUpdate();
	int CheckWinPcapUpdate();
	
	//账号密码
struct{
	u_char			m_username[50];
	int				m_usernameLen;
	u_char			m_password[50];
	int				m_passwordLen;
	char			m_ip[16];
	char			m_adaptername[256];
	u_char			m_MacAdd[6];			//存放MAC地址
	}User;
struct UserMsg{
	char AccountName[20], AccountTeam[20], BegainDate[20], EndDate[20], Address[20], Phone[12], Speed[20];
};
UserMsg Account = {};
	enum DEBUG_LEVEL{
		I_INFO,
		I_MSG,
		I_WARN,
		I_ERR,
		I_REM
	};
	void	Log(int level, const char *fmt, ...);
	void	UpdateStatus(bool bOnline);

	//线程控制
	HANDLE			m_AuthThread;			//认证的线程句柄,以便控制线程
	HANDLE			m_DHCPThread;			//DHCP的线程句柄,以便控制线程
	HANDLE			m_WebAuthThread;		//网页认证守护线程,以便控制线程
	HANDLE			m_WeblogoutThread;		//网页认证守护线程,以便控制线程
	HANDLE			m_HttpHeartThread;		//网页认证守护线程,以便控制线程
	HANDLE			m_NetFluxThread;		//网络状态守护线程,以便控制线程
	HANDLE			m_GetLANAccountMsgThread;
	void CloseThread();
	//中兴认证
	static void	get_packet(u_char *args, const struct pcap_pkthdr *pcaket_header, const u_char *packet);
	static unsigned WINAPI __stdcall eap_thread(void *para);
	static unsigned WINAPI __stdcall web_auth_thread(void *para);
	static unsigned WINAPI __stdcall logout_thread(void *para);
	static unsigned WINAPI __stdcall dhcp_thread(void *para);
	static unsigned WINAPI __stdcall http_heart_thread(void *para);

	//状态
	enum STATUS {
		INIT,
		AUTHING,
		AUTHED,
		DHCPING,
		DHCPED,
		HTTPING,
		HTTPED,
		ONLINE,
		OFFLINE
	};
	STATUS status;
	BOOL bConnected;
	BOOL bRetryConnect;
	void NetStatus();

	void FaultMsg(const char *msg);
	int PCsign(char *DiskSign);
	//网卡信息
	DWORD WINAPI GetMacIP(const char *adaptername, char *ip, unsigned char *mac);
	static char *	GetGUID(const char *name);


	afx_msg void OnIdok();
};
BOOL LoadImageFromResource(CImage *pImage, UINT nResID, LPCTSTR lpTyp);
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_AboutUs;
	afx_msg void OnBnClickedChkRestore();
};
class CComplaint : public CDialogEx
{
public:
	CComplaint(CWnd* pParent = NULL);
	
	// 对话框数据
	enum { IDD = IDD_Complaint };

protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
	CFont  fontTitle;
public:
	afx_msg void OnStnClickedGxerweima();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//信息获取
	static unsigned WINAPI __stdcall GetLANAccountMsg(void *para);
	CWnd *parent;
	void ShowMsg(BOOL bForce);
};
double GetDPIX(double X);
double GetDPIY(double Y);
void moveWindow(CWnd &wnd);
void SetDPI(int x, int y);