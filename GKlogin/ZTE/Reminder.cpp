#include "stdafx.h"
#include "GKlogin.h"
#include "GKloginDlg.h"
#include "AutoUpdate.h"
#include "BASE64.h"
#include <winioctl.h>
#include <md5.h>
#include <rc4.h>
#include <HyperLink.h>

#define MAX_BUFFER_SIZE 10204

int CGKloginDlg::Notice(int id)
{
	char http[MAX_BUFFER_SIZE] = { 0 };
	char *msg = (char *)malloc(MAX_BUFFER_SIZE);
	char url[MAX_BUFFER_SIZE] = { 0 };
	char title[MAX_BUFFER_SIZE] = { 0 };
	char content[MAX_BUFFER_SIZE] = { 0 };
	int ret = 0;
	for (int i = 0; i <= 2 && ret <= 0; i++){
		sprintf(url, LAN_URL  NoticeUp "?id=%d", id);
		ret = DownLoadFileToBuffer(http, MAX_BUFFER_SIZE, url, NULL, DOWNTIMEOUT, NULL, NULL, NULL, NULL);
		if (ret <= 0){
			sprintf(url, WEB_URL  NoticeUp "?id=%d", id);
			ret = DownLoadFileToBuffer(http, MAX_BUFFER_SIZE, url, NULL, DOWNTIMEOUT, NULL, NULL, NULL, NULL);
			if (ret <= 0)
			{
				Log(I_MSG, "无法显示公告，连接服务器失败！");
				continue;
			}
		}
		if (http)UTF8ToGBK((unsigned char*)http, (unsigned char*)http, MAX_BUFFER_SIZE);
		GetHTTPMsg(http, MAX_BUFFER_SIZE, 2, "Title:\"", "Content:\"", NULL, title, content, NULL, NULL, "\"");

		if (strlen(content)){
			SetBubble(title, content,3000);
			sprintf(msg, "%s:%s", title, content);

			Log(I_MSG, "%s", msg);
			break;
		}
	}
	Banner_Show();
	Config.SaveConfig();
	free(msg);
	return 0;
}
void CGKloginDlg::Banner_Show()
{
	char banner[MAX_BUFFER_SIZE]; int ret;
	char banner_url[MAX_BUFFER_SIZE];
	char http[MAX_BUFFER_SIZE] = { 0 };
	ret = DownLoadFileToBuffer(http, MAX_BUFFER_SIZE, LAN_URL  NoticeUrl, NULL, DOWNTIMEOUT, NULL, NULL, NULL, NULL);
	if (ret <= 0)
	{
		ret = DownLoadFileToBuffer(http, MAX_BUFFER_SIZE, WEB_URL  NoticeUrl, NULL, DOWNTIMEOUT, NULL, NULL, NULL, NULL);
		if (ret <= 0){
			Log(I_MSG, "无法获取BannerUrl，连接服务器失败！");
			return;
		}
	}
	GetHTTPMsg(http, MAX_BUFFER_SIZE, 2, "Banner_PNG:\"", "Banner_Url:\"", NULL, banner, banner_url, NULL, NULL, "\"");
	Config.banner_url = banner_url;

	char szTempPath[MAX_PATH], szTempName[MAX_PATH];
	GetTempPath(MAX_PATH, szTempPath);

	sprintf(szTempName, "%sBanner.png",szTempPath);

	ret = DownLoadFile(szTempName, banner, NULL, DOWNTIMEOUT, NULL, NULL);
	if (ret != 0)
	{
		Log(I_MSG, "无法获取Banner，连接服务器失败！");
		return;
	}
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
			Log(I_MSG, "Banner加载失败！");
		}
		else
		{
			if (BannerPicture != NULL)
				DeleteObject(BannerPicture);
		}
		CStatic* pWnd = (CStatic*)GetDlgItem(IDC_Banner);
		BannerPicture = Image.Detach();
		pWnd->SetBitmap(BannerPicture);
		pWnd->SetWindowPos(NULL,
			0,
			0,
			(int)GetDPIX(377),
			(int)GetDPIY(80),
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		//Image.Draw(pDC->m_hDC, 230, 110);
		Image.Destroy();
		ReleaseDC(pDC);
	}
}

void CGKloginDlg::OnStnClickedBanner()
{
	CHyperLink Link;
	//::ShellExecute(this->GetSafeHwnd(), _T("open"), banner_url, NULL, NULL, SW_SHOWNORMAL);
	Link.GotoURL(Config.banner_url, SW_SHOW);
}
void CGKloginDlg::FaultMsg(const char *msg)
{
	if (strlen(msg) != NULL)
	{
		Log(I_MSG, "");
		if (strstr(msg, "http") == 0)
		{
			if (strstr(msg, "错误码") != 0)
			{
				Log(I_MSG, "	中兴认证失败错误提示：");
				if (strstr(msg, "错误码110:用户在线数限制") != 0)
				{
					Log(I_MSG, "用户在线数限制，可能原因");
					Log(I_MSG, "   1.账号在别的电脑上登陆");
					Log(I_MSG, "   2.账号被盗用");
				}
				else if (strstr(msg, "错误码207:账号没有在该时段接入的权限") != 0)
				{
					Log(I_MSG, "账号没有在该时段接入的权限，可能原因");
					Log(I_MSG, "   1.断网账号，现在断网了");
				}
				else if (strstr(msg, "错误码101:不存在的用户") != 0)
				{
					Log(I_MSG, "账户输入错误，请重新输入您的账户！");
				}
				else if (strstr(msg, "错误码102:密码错误") != 0)
				{
					Log(I_MSG, "密码输入错误，请重新输入您的密码！");
				}
				else { Log(I_MSG, "%s，出错原因暂无收集，请到论坛反馈，以完善客户端", msg); }
			}
		}
	}
	else { Log(I_MSG, "服务器或交换机问题，请咨询天翼营业厅！"); }
	Log(I_MSG, "无法解决？请咨询天翼营业厅并论坛反馈以完善客户端");
	Log(I_MSG, "请等待1-2分钟后再次连接！");
	SetBubble("中兴认证失败", "无法解决？请咨询天翼营业厅并论坛反馈以完善客户端！", 15);
	if (logshowstatus)SendMessage(WM_COMMAND, MAKEWPARAM(IDC_LOGSHOW, 0));
}

void CGKloginDlg::WebFaultMsg(const char *msg)
{
	Log(I_MSG, "");
	Log(I_MSG, "	网页认证错误提示：");
	if (strstr(msg, "找不到SDX信息") != 0)
	{
		Log(I_MSG, "找不到SDX信息，可能原因：");
		Log(I_MSG, "   1.在设置里天翼认证打钩");
		Log(I_MSG, "   2.处于不需使用天翼网页认证的环境");
		Log(I_MSG, "   3.连接到了Wifi无法认证");
	}
	else if (strstr(msg, "没有定购此产品") != 0)
	{
		Log(I_MSG, "没有订购此产品，可能原因：");
		Log(I_MSG, "   1.新开账号开通中，请等待开通！");
		Log(I_MSG, "   2.如果绑定手机的账号，请检查手机是否欠费停机");
		Log(I_MSG, "   3.账号被停用");
		Log(I_MSG, "   4.电信服务器出现异常");
	}
	else if (strstr(msg, "网页认证连接超时") != 0)
	{
		Log(I_MSG, "网页认证连接超时，可能原因");
		Log(I_MSG, "   1.服务器抽风，请检查宿友是否同样情况");
		Log(I_MSG, "   2.检查网线连接是否正常");
		Log(I_MSG, "   3.上次网页认证未正常退出");
		Log(I_MSG, "   4.未进行中兴认证");
		Log(I_MSG, "   5.检查设置DHCP是否已经打开");
		Log(I_MSG, "   6.检查本地连接DNS服务器设置是否为自动获取");
	}
	else if (strstr(msg, "用户认证失败") != 0)
	{
		Log(I_MSG, "用户认证失败，可能原因");
		Log(I_MSG, "   1.用户没有下线账号而再次登录，请等待几分钟");
	}
	else { Log(I_MSG, "%s，出错原因暂无收集，请到论坛反馈，以完善客户端", msg); }
	Log(I_MSG, "无法解决？请咨询天翼营业厅并论坛反馈以完善客户端\r\n");
	SetBubble("网页认证失败", "无法解决？请咨询天翼营业厅并论坛反馈以完善客户端", 15);
	if (logshowstatus)SendMessage(WM_COMMAND, MAKEWPARAM(IDC_LOGSHOW, 0));
}

//滤除字符串起始位置的空格
void TrimStart(LPTSTR pBuf)
{
	if (*pBuf != 0x20)
		return;

	LPTSTR pDest = pBuf;
	LPTSTR pSrc = pBuf + 1;
	while (*pSrc == 0x20)
		++pSrc;

	while (*pSrc)
	{
		*pDest = *pSrc;
		++pDest;
		++pSrc;
	}
	*pDest = 0;
}

//把WORD数组调整字节序为little-endian，并滤除字符串结尾的空格。
void ToLittleEndian(PUSHORT pWords, int nFirstIndex, int nLastIndex, LPTSTR pBuf)
{
	int index;
	LPTSTR pDest = pBuf;
	for (index = nFirstIndex; index <= nLastIndex; ++index)
	{
		pDest[0] = pWords[index] >> 8;
		pDest[1] = pWords[index] & 0xFF;
		pDest += 2;
	}
	*pDest = 0;

	//trim space at the endof string; 0x20: _T(' ')
	--pDest;
	while (*pDest == 0x20)
	{
		*pDest = 0;
		--pDest;
	}
}
//
// Model Number: 40 ASCII Chars
// SerialNumber: 20 ASCII Chars
//
BOOL GetPhyDriveSerial(LPTSTR pModelNo, LPTSTR pSerialNo)
{
	//-1是因为 SENDCMDOUTPARAMS 的结尾是 BYTE bBuffer[1];
	BYTE IdentifyResult[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
	DWORD dwBytesReturned;
	GETVERSIONINPARAMS get_version;
	SENDCMDINPARAMS send_cmd = { 0 };

	HANDLE hFile = CreateFile(_T("\\\\.\\PHYSICALDRIVE0"), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	//get version
	DeviceIoControl(hFile, SMART_GET_VERSION, NULL, 0,
		&get_version, sizeof(get_version), &dwBytesReturned, NULL);

	//identify device
	send_cmd.irDriveRegs.bCommandReg = (get_version.bIDEDeviceMap & 0x10) ? ATAPI_ID_CMD : ID_CMD;
	DeviceIoControl(hFile, SMART_RCV_DRIVE_DATA, &send_cmd, sizeof(SENDCMDINPARAMS) - 1,
		IdentifyResult, sizeof(IdentifyResult), &dwBytesReturned, NULL);
	CloseHandle(hFile);

	//adjust the byte order
	PUSHORT pWords = (USHORT*)(((SENDCMDOUTPARAMS*)IdentifyResult)->bBuffer);
	ToLittleEndian(pWords, 27, 46, pModelNo);
	ToLittleEndian(pWords, 10, 19, pSerialNo);
	return TRUE;
}

int CGKloginDlg::PCsign(char *DiskSign) {
	TCHAR szModelNo[48], szSerialNo[24];
	if (GetPhyDriveSerial(szModelNo, szSerialNo))
	{
		//_tprintf(_T("Model No: %s\n"), szModelNo);
		TrimStart(szSerialNo);
		//_tprintf(_T("Serial No: %s\n"), szSerialNo);
	}

	MD5_CTX mdContext;
	int bytes;
	unsigned char data[1024];
	char md5[33];
	int i;

	int md5_len = 16;
	strcpy((char *)data, szSerialNo);
	bytes = strlen((char *)data);

	static u_char digest[16];

	MD5Init(&mdContext);
	if (strcmp((char *)data, "") != 0)
	{
		MD5Update(&mdContext, data, bytes);
	}
	MD5Final(digest, &mdContext);


	memset(md5, 0, (md5_len + 1));

	if (md5_len == 16)
	{
		for (i = 4; i<12; i++)
		{
			sprintf(&md5[(i - 4) * 2], "%02X", digest[i]);
		}
	}
	if (strcmp((char *)md5, "") == 0)
	{
		//fprintf(stderr, "failed.\n");
		return NULL;
	}
	//_tprintf(_T("SN Hash: %s\n"), md5);
	strcpy(DiskSign, md5);
	return 1;
}