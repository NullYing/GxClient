#include "stdafx.h"
#include "ChangePWD.h"
#include "SettingDlg.h"
#include "authcode.h"
extern CString edubas;

CChangePWD::CChangePWD(CWnd* pParent /*=NULL*/)
	: CDialogEx(CChangePWD::IDD, pParent)
{
	
}

BOOL CChangePWD::OnInitDialog()
{
	((CButton *)GetDlgItem(IDC_PWDChange))->SetCheck(TRUE);
	return TRUE;
}
void CChangePWD::OnOK()
{
	GetDlgItem(IDC_AccountChange)->GetWindowText(Account, MAX_STRING);
	GetDlgItem(IDC_PWDChangeOld)->GetWindowText(PassWordOld, MAX_STRING);
	GetDlgItem(IDC_PWDChangeNew1)->GetWindowText(PassWordNew1, MAX_STRING);
	GetDlgItem(IDC_PWDChangeNew2)->GetWindowText(PassWordNew2, MAX_STRING);
	if (strlen(Account)==0)
	{
		AfxMessageBox("用户名不能为空！", MB_OK | MB_ICONEXCLAMATION| MB_TOPMOST);
		return;
	}
	if (strlen(PassWordOld) == 0)
	{
		AfxMessageBox("旧密码不能为空", MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
		return;
	}
	if (strlen(PassWordNew1) == 0)
	{
		AfxMessageBox("新密码不能为空", MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
		return;
	}
	if (strcmp(PassWordNew1 , PassWordNew2)!=0)
	{
		AfxMessageBox("输入的新密码不一致，请重新输入！", MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
		return;
	}
	if (strcmp(PassWordOld, PassWordNew1) == 0)
	{
		AfxMessageBox("输入新密码和旧密码相同，请重新输入！", MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
		return;
	}
	int choose=0;
	if (((CButton *)GetDlgItem(IDC_ZTEPWDChange))->GetCheck())
		choose = 1;
	else if (((CButton *)GetDlgItem(IDC_WebPWDChange))->GetCheck())
		choose = 2;
	else if (((CButton *)GetDlgItem(IDC_PWDChange))->GetCheck())
		choose = 3;
	int result = AfxMessageBox(_T("请确认是否修改密码？"), MB_ICONQUESTION | MB_YESNO | MB_TOPMOST);
	if (result == IDYES){
		switch (choose){
		case 1:
			ZTEChangePassWord();
			break;
		case 2:
			WebChangePassWord();
			break;
		case 3:
			if (ZTEChangePassWord() == 1)
				WebChangePassWord();
			break;
		default:
			AfxMessageBox("请选择一个修改选项！", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
			break;
		}
	}
}
int CChangePWD::WebChangePassWord()
{
	//char *msg = (char *)malloc(MAX_BUFFER_SIZE);
	
	char msg[MAX_BUFFER_SIZE] = { 0 };
	char http[MAX_BUFFER_SIZE] = { 0 };
	char http2[MAX_BUFFER_SIZE] = { 0 };
	char data[MAX_BUFFER_SIZE] = { 0 };
	char ip[16] = {0};
	CGKloginDlg zte;
#ifdef WIN32
#define snprintf _snprintf
#endif
	zte.GetMacIP(Config.m_csNetCard, ip, NULL);
	if (strlen(edubas) == 0)
		snprintf(data, MAX_BUFFER_SIZE, "&userName1=%s&password1=%s&password2=%s&password3=%s&eduuser=%s&edubas=113.98.13.29", Account, PassWordOld, PassWordNew1, PassWordNew2, ip);
	else
		snprintf(data, MAX_BUFFER_SIZE, "&userName1=%s&password1=%s&password2=%s&password3=%s&eduuser=%s&edubas=%s", Account, PassWordOld, PassWordNew1, PassWordNew2, ip, edubas);
	SendPost(http2, MAX_BUFFER_SIZE, "http://125.88.59.131:10001/changePassword.do", NULL, DOWNTIMEOUT, NULL, NULL, NULL, data);
	UTF8ToGBK((unsigned char*)http2, (unsigned char*)http, MAX_BUFFER_SIZE);
	GetHTTPMsg(http, MAX_BUFFER_SIZE, 1, "<div id=\"success\">", NULL, NULL, msg, NULL, NULL, NULL, "<div>");
	if (strlen(msg)){
		char Remind[MAX_BUFFER_SIZE] = { 0 };
		sprintf(Remind, "网页认证密码%s", msg);
		AfxMessageBox(Remind, MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		if (strstr(Remind, "成功") != 0)
			return 1;
	}
	else
	{ 
		AfxMessageBox("网页认证密码修改失败", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}
	//free(msg);
	return 0;
}

int CChangePWD::ZTEChangePassWord()
{
	char http[MAX_BUFFER_SIZE] = { 0 };
	char data[MAX_BUFFER_SIZE] = { 0 };
	char msg[MAX_BUFFER_SIZE] = { 0 };
	char text[5];
	char szTempPath[MAX_PATH], cookies[MAX_PATH], szTempName[MAX_PATH];
	GetTempPath(MAX_PATH, szTempPath);
	GetTempFileName(szTempPath, _T("CZTE"), 0, cookies);

	//获取验证码和cookies
	GetTempFileName(szTempPath, _T("Code"), 0, szTempName);
	strcat(szTempName, ".png");
	char TempUrl[MAX_STRING];
	sprintf(TempUrl, "%s/servlet/ValidateCodeServlet", Config.m_csZTEServer);
	int ifSeccess = 0;
	for (int i = 1; i <= 3; i++)
	{
		if (ifSeccess)break;
		sprintf(TempUrl, "%s/servlet/ValidateCodeServlet", Config.m_csZTEServer);
		if (DownLoadFile(szTempName, TempUrl, NULL, DOWNTIMEOUT, NULL, cookies) == 0){
			authcode(szTempName, text);
			sprintf(data, "userName=%s&userPassword=%s&validateCode=%s&Submit3=%%C8%%B7+++%%B6%%A8", Account, PassWordOld, text);
			sprintf(TempUrl, "%s/user/userLogin.action", Config.m_csZTEServer);
			SendPost(http, MAX_BUFFER_SIZE, TempUrl, NULL, DOWNTIMEOUT, NULL, cookies, NULL, data);
			ifSeccess = GetHTTPMsg(http, MAX_BUFFER_SIZE, 1, "欢迎您", NULL, NULL, msg, NULL, NULL, NULL, "<LI>");
		}
		printf("重试次数：%d\n",i);
	}
	if (ifSeccess)
	{
		printf("\n登录完成");
		//拿下post必须提交信息
		char includemsg1[MAX_STRING], includemsg2[MAX_STRING], includemsg3[MAX_STRING], includemsg4[MAX_STRING], includemsg5[MAX_STRING], includemsg6[MAX_STRING], includemsg7[MAX_STRING], includemsg8[MAX_STRING];
		sprintf(TempUrl, "%s/user/editUserPwd.action", Config.m_csZTEServer);
		if (DownLoadFileToBuffer(http, MAX_BUFFER_SIZE, TempUrl, NULL, DOWNTIMEOUT, NULL, cookies, NULL,NULL) >= 0){
			GetHTTPMsg(http, MAX_BUFFER_SIZE, 3, "isSuccess", "pop", "queryReturnURL", includemsg1, includemsg2, includemsg3, "value=\"", "\"");
			GetHTTPMsg(http, MAX_BUFFER_SIZE, 3, "userVo.startDate", "userVo.endDate", "userVo.userType", includemsg4, includemsg5, includemsg6, "value=\"", "\"");
			GetHTTPMsg(http, MAX_BUFFER_SIZE, 2, "struts.token.name", "struts.token\"", NULL, includemsg7, includemsg8, NULL, "value=\"", "\"");
			//printf(http);
			//printf("\n阶段二");
			//提交修改密码
			sprintf(data, "isSuccess=%s&pop=%s&queryReturnURL=%s&userVo.startDate=%s&userVo.endDate=%s&userVo.userType=%s&struts.token.name=%s&struts.token=%s&oldPassword=%s&newPassword=%s&reNewPassword=%s", includemsg1, includemsg2, includemsg3, includemsg4, includemsg5, includemsg6, includemsg7, includemsg8, PassWordOld, PassWordNew1, PassWordNew2);
			//printf(data);
			sprintf(TempUrl, "%s/user/updatePwd.action", Config.m_csZTEServer);
			SendPost(http, MAX_BUFFER_SIZE, TempUrl, NULL, DOWNTIMEOUT, NULL, cookies, NULL, data);
			GetHTTPMsg(http, MAX_BUFFER_SIZE, 1, "msgSuccess", NULL, NULL, msg, NULL, NULL, "<dt>", "</dt>");
			//printf(http);
			if (strlen(msg))
			{
				char Remind[MAX_BUFFER_SIZE] = { 0 };
				sprintf(Remind, "中兴认证密码修改%s", msg);
				AfxMessageBox(Remind, MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
				if (strstr(Remind, "成功") != 0)
					return 1;
			}
		}
	}
	else
	{
		AfxMessageBox("登陆失败", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}
	DeleteFile(cookies);
	DeleteFile(szTempName);
	return 0;
}