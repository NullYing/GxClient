#include "stdafx.h"
#include "AutoUpdate.h"
#include "WebAuth.h"
#include "GKloginDlg.h"
#include "SettingDlg.h"
#include "BASE64.h"
#include "authcode.h"
#define MAX_BUFFER_SIZE 50480
extern CBase64 Base64;

void CGKloginDlg::GetAccountMsg()
{
	char data[MAX_BUFFER_SIZE];
	if (Config.m_CYClient)
	{ 
		sprintf(data, "productSpcName=%s&productSpcId=216", URLEncode(Base64.GBToUTF8("东莞理工学院城市学院（寮步校区）")));
	}
	else{
		sprintf(data, "productSpcName=%s&productSpcId=212", URLEncode(Base64.GBToUTF8("广东科技学院")));
	}
	char data2[MAX_BUFFER_SIZE] = { 0 };
	if (Config.m_bEnableWebAccount)
		sprintf(data2, "&externalNumber=%s&password=%s", Config.m_csWebUsername, Config.m_csWebPassword);
	else
		sprintf(data2, "&externalNumber=%s&password=%s", User.m_username, User.m_password);
	strcat(data, data2);
	char http[MAX_BUFFER_SIZE] = { 0 };
	char Status[MAX_BUFFER_SIZE] = { 0 };
	char msg[MAX_BUFFER_SIZE] = { 0 };
	char szTempPath[MAX_PATH], szTempName[MAX_PATH];
	char *p = NULL;
	GetTempPath(MAX_PATH, szTempPath);
	GetTempFileName(szTempPath, _T("WebCookies"), 0, szTempName);

	SendPost(http, MAX_BUFFER_SIZE, URL_ChangeTYPassword "/login.do", NULL, DOWNTIMEOUT, NULL, NULL, szTempName, data);
	//UTF8ToGBK((unsigned char*)http, (unsigned char*)msg, MAX_BUFFER_SIZE);
	int ret = DownLoadFileToBuffer(http, MAX_BUFFER_SIZE, URL_ChangeTYPassword "/baseInfo.do?basicinfo=productinfo", NULL, DOWNTIMEOUT, NULL, szTempName, NULL,NULL);
	if (ret >= 0)
	{
		UTF8ToGBK((unsigned char*)http, (unsigned char*)msg, MAX_BUFFER_SIZE);
		if (msg)
		{
			GetHTTPMsg(msg, MAX_BUFFER_SIZE, 2, "用户速率：", "用户状态：", NULL, Account.Speed, Status, NULL, "<td class=\"tabxix\">", "&nbsp;");
			if (strcmp(Account.Speed, "") != 0)
			{
				sprintf(msg, "用户速率：%s", Account.Speed + 3);
				m_StatusBar.SetText(msg, 4, 0);
				Log(I_MSG, msg);
				sprintf(msg, "用户状态：%s", Status);
				Log(I_MSG, msg);
			}
		}
	}
	else
		Log(I_MSG, "获取用户速率失败！");
	DeleteFile(szTempName);
}
unsigned WINAPI __stdcall CComplaint::GetLANAccountMsg(void *para)
{
	CGKloginDlg *Dlg = (CGKloginDlg*)para;
	char http[MAX_BUFFER_SIZE] = { 0 };
	char data[MAX_BUFFER_SIZE] = { 0 };
    char msg[MAX_BUFFER_SIZE] = { 0 };
	char text[5];
	char szTempPath[MAX_PATH], cookies[MAX_PATH], szTempName[MAX_PATH];
	GetTempPath(MAX_PATH, szTempPath);
	GetTempFileName(szTempPath, _T("ZTELANCookies"), 0, cookies);
	GetTempFileName(szTempPath, _T("ValidateCode"), 0, szTempName);
	//strcat(szTempName, ".png");
	char TempUrl[MAX_STRING];
	int ret;
	for (int i = 1; i <= 3 ; i++){
		sprintf(TempUrl, "%s/servlet/ValidateCodeServlet", Config.m_csZTEServer);
		ret = DownLoadFile(szTempName, TempUrl, NULL, DOWNTIMEOUT, NULL, cookies);
		if (ret == 0){
			authcode(szTempName, text);
			//printf(text);
			//printf("\n");
			sprintf(data, "userName=%s&userPassword=%s&validateCode=%s", Dlg->User.m_username, Dlg->User.m_password, text);
			sprintf(TempUrl, "%s/user/userLogin.action", Config.m_csZTEServer);
			SendPost(http, MAX_BUFFER_SIZE, TempUrl, NULL, DOWNTIMEOUT, NULL, cookies, NULL, data);
			sprintf(TempUrl, "%s/user/queryUser.action", Config.m_csZTEServer);
			ret = DownLoadFileToBuffer(http, MAX_BUFFER_SIZE, TempUrl, NULL, DOWNTIMEOUT, NULL, cookies, NULL,NULL);
			//printf(http);
			if (ret > 0)
			{
				ret = GetHTTPMsg(http, MAX_BUFFER_SIZE, 3, "开户日期", "账号截止日期", "用户姓名", Dlg->Account.BegainDate, Dlg->Account.EndDate, Dlg->Account.AccountName, "bgcolor=\"#FFFFFF\">", "</td>");
				//printf("%s  %s  %s\n", Dlg->Account.BegainDate, Dlg->Account.EndDate, Dlg->Account.AccountName);
				//printf(http);
				ret=GetHTTPMsg(http, MAX_BUFFER_SIZE, 3, "用户组", "用户地址", "联系电话", Dlg->Account.AccountTeam, Dlg->Account.Address, Dlg->Account.Phone, "bgcolor=\"#FFFFFF\">", "</td>");
				//printf("%s  %s  %s", Dlg->Account.AccountTeam, Dlg->Account.Address, Dlg->Account.Phone);
				if (ret = 3)
					break;
			}
		}
		Sleep(500);
	}
	DeleteFile(cookies);
	DeleteFile(szTempName);
	Dlg->m_GetLANAccountMsgThread = NULL;
	return 0;
}
void CComplaint::ShowMsg(BOOL bForce = FALSE)
{
	CGKloginDlg *Dlg = (CGKloginDlg *)parent;
	if ((strlen(_T(Dlg->Account.AccountName)) > 1) || bForce)
	{
		GetDlgItem(IDC_Account)->SetWindowText(_T((char *)Dlg->User.m_username));
		GetDlgItem(IDC_AccountName)->SetWindowText(_T(Dlg->Account.AccountName));
		GetDlgItem(IDC_AccountTeam)->SetWindowText(_T(Dlg->Account.AccountTeam));
		GetDlgItem(IDC_AccountAddress)->SetWindowText(_T(Dlg->Account.Address));
		GetDlgItem(IDC_AccountPhone)->SetWindowText(_T(Dlg->Account.Phone));
		GetDlgItem(IDC_AccountBegainDate)->SetWindowText(_T(Dlg->Account.BegainDate));
		GetDlgItem(IDC_AccountEndDate)->SetWindowText(_T(Dlg->Account.EndDate));
	}
	else
	{
		GetDlgItem(IDC_Account)->SetWindowText(_T("无信息"));
		GetDlgItem(IDC_AccountName)->SetWindowText(_T("无信息"));
		GetDlgItem(IDC_AccountTeam)->SetWindowText(_T("无信息"));
		GetDlgItem(IDC_AccountAddress)->SetWindowText(_T("无信息"));
		GetDlgItem(IDC_AccountPhone)->SetWindowText(_T("无信息"));
		GetDlgItem(IDC_AccountBegainDate)->SetWindowText(_T("无信息"));
		GetDlgItem(IDC_AccountEndDate)->SetWindowText(_T("无信息"));
	}
}