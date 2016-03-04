
#include "stdafx.h"
#include "GKloginDlg.h"
#include "AutoUpdate.h"
#define MAX_BUFFER_SIZE 10204
static char logmsg[MAX_BUFFER_SIZE];

unsigned __stdcall CGKloginDlg::NetFlux(void *para)
{
	CGKloginDlg *Dlg = (CGKloginDlg*)para;
	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];
	//struct timeval st_ts;
	//u_int netmask;
	//struct bpf_program fcode;

	/* 检查命令行参数的合法性 */


	/* 打开输出适配器 */
	if ((fp = pcap_open_live(ToNPFName(Config.m_csNetCard), 100, 1, 1000, errbuf)) == NULL)
	{
		Dlg->m_StatusBar.SetText(_T("\nUnable to open adapter %s."), 3, 0);
		
		return 0;
	}

	/* 不用关心掩码，在这个过滤器中，它不会被使用 */
	//netmask = 0xffffff;

	// 编译过滤器
	//if (pcap_compile(fp, &fcode, "tcp"/*我用的是ADSL这里是设置只接收PPPOE的包*/, 1, netmask) <0)
	//{
	//	fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		/* 释放设备列表 */
	//	return 0;
	//}

	//设置过滤器
	//if (pcap_setfilter(fp, &fcode)<0)
	//{
	//	fprintf(stderr, "\nError setting the filter.\n");
	//	pcap_close(fp);
		/* 释放设备列表 */
	//	return 0;
	//}

	/* 将接口设置为统计模式 */
	if (pcap_setmode(fp, MODE_STAT)<0)
	{
		Dlg->m_StatusBar.SetText(_T("\nError setting the mode."), 3, 0);
		pcap_close(fp);
		/* 释放设备列表 */
		return 0;
	}

	/* 开始主循环 */
	pcap_loop(fp, 0, CGKloginDlg::dispatcher_handler, (u_char*)Dlg);

	pcap_close(fp);
	Dlg->m_StatusBar.SetText("", 3, 0);
	Dlg->m_NetFluxThread = NULL;
	return 0;
}

void CGKloginDlg::dispatcher_handler(u_char *state, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	CGKloginDlg *Dlg = (CGKloginDlg*)state;
	//struct timeval *old_ts = (struct timeval *)state;
	u_int delay;
	LARGE_INTEGER Bps, Pps;
	//struct tm *ltime;
	//char timestr[16];
	time_t local_tv_sec;

	/* 以毫秒计算上一次采样的延迟时间 */
	/* 这个值通过采样到的时间戳获得 */
	delay = (header->ts.tv_sec - Dlg->tv_sec) * 1000000 - Dlg->tv_usec + header->ts.tv_usec;
	/* 获取每秒的比特数b/s */
	Bps.QuadPart = (((*(LONGLONG*)(pkt_data + 8)) * 8 * 1000000) / (delay));
	/*                                            ^      ^
	|      |
	|      |
	|      |
	将字节转换成比特 --   |
	|
	延时是以毫秒表示的 --
	*/

	/* 得到每秒的数据包数量 */
	Pps.QuadPart = (((*(LONGLONG*)(pkt_data)) * 1000000) / (delay));

	/* 将时间戳转化为可识别的格式 */
	local_tv_sec = header->ts.tv_sec;
	//ltime = localtime(&local_tv_sec);
	//strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);

	/* 打印时间戳*/
	sprintf(logmsg,"实时网速：KBPS=%.1lf PPS=%I64u", (float)Bps.QuadPart/8192, Pps.QuadPart);
	Dlg->Flux = (float)Bps.QuadPart / 8192;
	Dlg->m_StatusBar.SetText(_T(logmsg), 3, 0);
	/* 打印采样结果 */
	//printf("BPS=%I64u ", );
	//printf("\n", );
	
	//存储当前的时间戳
	Dlg->tv_sec = header->ts.tv_sec;
	Dlg->tv_usec = header->ts.tv_usec;

}
void CGKloginDlg::NetStatus()
{
	static int ErrorNum = 0; int ret;
	long retcode = 0;
	ret = DownLoadFileToBuffer(NULL, 0, Config.m_csHeartUrl, User.m_ip, DOWNTIMEOUT, Config.m_csHeartCookies, NULL, NULL, &retcode);
	if (ret < NULL&&ret != -3 && ret != -10)
		Log(I_MSG, "网络状态:%d", ret);
	if (ret == -1 || ret == -2 || ret == -4){
		ErrorNum++;
	}
	else{
		ErrorNum = 0;
	}
	if (retcode == 302)
	{
		Log(I_MSG, "检测到网页认证异常！");
		m_HttpHeartThread = NULL;
		if (Config.m_bWebAuth)
			StartWebAuth();
	}
	if (ErrorNum >= 2){
		Log(I_MSG, "检测到认证异常！");
		bConnected = 0;
		ErrorNum = 0;
	}
	else{
		bConnected = 1;
	}
}

//////////////////////////////////////////////////////////////////////////
//自动获取IP
//////////////////////////////////////////////////////////////////////////
typedef BOOL(WINAPI *DHCPNOTIFYPROC)(
	LPWSTR lpwszServerName, // 本地机器为NULL
	LPWSTR lpwszAdapterName, // 适配器名称
	BOOL bNewIpAddress, // TRUE表示更改IP
	DWORD dwIpIndex, // 指明第几个IP地址，如果只有该接口只有一个IP地址则为0
	DWORD dwIpAddress, // IP地址
	DWORD dwSubNetMask, // 子网掩码
	int nDhcpAction); // 对DHCP的操作 0:不修改, 1:启用 DHCP，2:禁用 DHCP


bool CGKloginDlg::EnableDHCP(const char* szAdapterName, const bool enable)
{
	bool			bResult = false;
	HINSTANCE		hDhcpDll;
	DHCPNOTIFYPROC	pDhcpNotifyProc;
	WCHAR wcAdapterName[256];

	//mbstowcs(wcAdapterName, szAdapterName, strlen(szAdapterName));
	MultiByteToWideChar(CP_ACP, 0, szAdapterName, -1, wcAdapterName, 256);

	if ((hDhcpDll = LoadLibrary(_T("dhcpcsvc.dll"))) == NULL)
		return false;

	if ((pDhcpNotifyProc = (DHCPNOTIFYPROC)GetProcAddress(hDhcpDll, "DhcpNotifyConfigChange")) != NULL) {
		if (pDhcpNotifyProc(NULL, wcAdapterName, FALSE, 0, 0, 0, enable ? 1 : 2) == ERROR_SUCCESS)
		{
			bResult = true;
		}
	}
	FreeLibrary(hDhcpDll);
	return bResult;
}
static int DHCP_Auto_Restart()
{
	char command[150];
	HKEY hKey;
	char RegEnableDHCP2[300];
	TCHAR sz[20];
	DWORD dwSize = sizeof(sz);
	HINSTANCE ERRORMSG;
	for (int k = 1, j = 1; k < 003; k++, j++)
	{
		sprintf(RegEnableDHCP2, "SYSTEM\\ControlSet00%d\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\%s\\Connection", k, Config.m_csNetCard);
		LONG errCode = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			RegEnableDHCP2,
			NULL, KEY_ALL_ACCESS, &hKey);

		if (RegQueryValueEx(hKey, _T("Name"), NULL, NULL, (LPBYTE)sz, &dwSize) == ERROR_SUCCESS)
		{
			sprintf(command, "interface ip set address name=\"%s\" source=dhcp", sz);
			ERRORMSG = ShellExecute(NULL, _T("open"), "netsh.exe", command, _T(""), SW_HIDE);
			sprintf(command, "interface set interface name=\"%s\" admin=DISABLED", sz);
			ERRORMSG = ShellExecute(NULL, _T("open"), "netsh.exe", command, _T(""), SW_HIDE);
			if ((int)ERRORMSG > 32)
			{
				Sleep(10000);
				sprintf(command, "interface set interface name=\"%s\" admin=ENABLED", sz);
				ERRORMSG = ShellExecute(NULL, _T("open"), "netsh.exe", command, _T(""), SW_HIDE);
				if ((int)ERRORMSG > 32)Sleep(6000);
			}
			RegCloseKey(hKey);
			return 0;
		}
	}
	RegCloseKey(hKey);
	return 1;
}
int CGKloginDlg::DHCP_Auto()
{
	int Sign = 0;
	//自动更改为自动获取ip
	if (Config.m_bDHCP)
	{
		HKEY DefaultGateway;
		HKEY DHCPIP;
		char Reg[150];
		DWORD regsz = REG_DWORD;
		DWORD pjPath = 1;
		DWORD iPathLen;

		for (int k = 1, j = 1; k < 003; k++, j++)
		{
			sprintf(Reg, "SYSTEM\\ControlSet00%d\\services\\Tcpip\\Parameters\\Interfaces\\%s", k, Config.m_csNetCard);
			//改为自动获取
			LONG errCode = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				Reg,
				NULL, KEY_ALL_ACCESS, &DHCPIP);
			iPathLen = sizeof(DWORD);
			errCode = ::RegQueryValueEx(DHCPIP, _T("EnableDHCP"), NULL, &regsz, (unsigned char*)&pjPath, &iPathLen);
			if (errCode == ERROR_SUCCESS && pjPath != 1)
			{
				pjPath = 1;
				errCode = ::RegSetValueEx(DHCPIP, _T("EnableDHCP"), NULL, REG_DWORD, (unsigned char*)&pjPath, sizeof(DWORD));
				if (errCode == ERROR_SUCCESS)
				{
					//删除网关
					errCode = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						Reg,
						NULL, KEY_ALL_ACCESS, &DefaultGateway);
					RegDeleteValue(DefaultGateway, _T("DefaultGateway"));
					Log(I_MSG, "设置网卡已自动设置为自动获取ip");
					Sign = 1;
				}
				else
				{
					Log(I_ERR, "设置网卡已自动设置为自动获取ip失败，请手动设置！");
					RegCloseKey(DHCPIP);
					return 0;
				}
			}
		}
		RegCloseKey(DHCPIP);
		if (Sign)
		{
			ShowWindow(SW_HIDE);
			SetBubble("提示", STR_AppName" 正在重启网卡中……");
			if (DHCP_Auto_Restart()){
				Log(I_ERR, "自动重启网卡失败，请手动重启网卡或拔出网线重新插入！");
				return 0;
			}
			else
			{
				Log(I_MSG, "自动重启网卡成功,自动重新认证！");
			}
		}
	}
	return 1;
}
int CGKloginDlg::WebPop()
{
	HKEY ActiveProbing;
	char Reg[150];
	DWORD regsz = REG_DWORD;
	DWORD pjPath = 1;
	DWORD iPathLen;

	for (int k = 1, j = 1; k < 003; k++, j++)
	{
		sprintf(Reg, "SYSTEM\\ControlSet00%d\\services\\NlaSvc\\Parameters\\Internet", k);
		//改为自动获取
		LONG errCode = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			Reg,
			NULL, KEY_ALL_ACCESS, &ActiveProbing);
		iPathLen = sizeof(DWORD);
		errCode = ::RegQueryValueEx(ActiveProbing, _T("EnableActiveProbing"), NULL, &regsz, (unsigned char*)&pjPath, &iPathLen);
		if (errCode == ERROR_SUCCESS && pjPath != 0)
		{
			pjPath = 0;
			errCode = ::RegSetValueEx(ActiveProbing, _T("EnableActiveProbing"), NULL, REG_DWORD, (unsigned char*)&pjPath, sizeof(DWORD));
			if (errCode == ERROR_SUCCESS)
			{
				Log(I_MSG, "设置关闭自动弹出网页认证窗口成功");
			}
			else
			{
				Log(I_ERR, "关闭自动弹出网页认证窗口失败！");
			}
		}
	}
	RegCloseKey(ActiveProbing);
	return 1;
}