#include "stdafx.h"
#include "AutoUpdate.h"
#include "WebAuth.h"
#include "Config.h"
extern CConfig Config;
#define MAX_BUFFER_SIZE 10240

char *Get_Jump(const char *ip, const int timeout, const char * test_url, CString &jump_ip)
{
	static char jump_http[MAX_BUFFER_SIZE], url[MAX_BUFFER_SIZE] = { 0 };
	long retcode;
	int ret = DownLoadFileToBuffer(NULL, 0, test_url, NULL, timeout, NULL, NULL, url, &retcode);
	if (retcode == 302){
		char *p = strstr(url, "wlanacip=") + strlen("wlanacip=");
		char *q = strchr(p, '&');
		if (q != NULL) {
			*q = '\0';
		}
		jump_ip = p;
		p = strchr(url, ':');
		if (p != NULL) {
			p = strchr(p + 1, ':');
			if (p != NULL)
				*p = '\0';
		}
		return jump_ip.GetBuffer();
	}
	if (retcode == 200)
		return url;
	else
		return NULL;
}

static char jump_http[MAX_STRING];
CString edubas;

char * WebAuth(const char * username, const char *password, const char *ip,const char *base_url, const int timeout, const char * test_url)
{	

	//ip = NULL;//测试用

	char url[MAX_BUFFER_SIZE] = { 0 }; char AuthUrl[MAX_STRING] = { 0 };

	if (Config.m_AutoBas || edubas.GetLength() == 0) {
		char *http = Get_Jump(ip, timeout, test_url, edubas);
		if (http) {
			if (strstr(http, test_url) != 0)
				return NULL;
			if (strstr(edubas, "113.98.10.136") != 0)
				Config.m_CYClient = 1;
			else
				Config.m_CYClient = 0;
			strcpy(jump_http, http);
			_snprintf(url, MAX_BUFFER_SIZE, "%s/login.do?edubas=%s", jump_http, edubas);
			strcpy(AuthUrl, url);
			Config.m_csWebAuthUrl = AuthUrl;
			_snprintf(url, MAX_BUFFER_SIZE, "%s&eduuser=%s&userName1=%s&password1=%s", AuthUrl, ip, username, password);
		}
		else
			_snprintf(url, MAX_BUFFER_SIZE, "%s&eduuser=%s&userName1=%s&password1=%s", base_url, ip, username, password);
	}
	else
		_snprintf(url, MAX_BUFFER_SIZE, "%s&eduuser=%s&userName1=%s&password1=%s", base_url, ip, username, password);

	//memset(http, 0, MAX_BUFFER_SIZE*sizeof(char));

	return WebAuthMessages(url,ip,timeout);
}

char *WebAuthMessages(const char url[MAX_BUFFER_SIZE], const char *ip, const int timeout)
{
	static char msg[MAX_BUFFER_SIZE];
	char http[MAX_BUFFER_SIZE] = { 0 };
	char Getmsg[MAX_BUFFER_SIZE] = { 0 }, jump_url[MAX_BUFFER_SIZE] = { 0 };
 
	int ret = DownLoadFileToBuffer(http, MAX_BUFFER_SIZE, url, ip, timeout, NULL, NULL, jump_url,NULL);
	if (ret <= 0){
		sprintf(msg,"网页认证连接超时，错误代码：%d",ret);
		return msg;
	}

	if (strstr(jump_url, "/success.jsp") != NULL)
		return NULL;
	else{
		if (!GetHTTPMsg(http, MAX_BUFFER_SIZE, 1, "alertInfo = \"", NULL, NULL, Getmsg, NULL, NULL, NULL, "\""))
		{
			if (!GetHTTPMsg(http, MAX_BUFFER_SIZE, 1, "<div id=\"failed\">", NULL, NULL, Getmsg, NULL, NULL, NULL, "</div>"))
				return "无法获取信息！";
		}
	}
	if (strcmp(Getmsg, "null") == 0) return NULL;

	UTF8ToGBK((unsigned char*)Getmsg, (unsigned char*)msg, MAX_BUFFER_SIZE);
	return msg;
}
char * WebLogout(const char *ip, const char *base_url, const int timeout)
{
	//ip = "110.101.48.22";//测试用

	static char msg[MAX_BUFFER_SIZE];
	char jump_url[MAX_BUFFER_SIZE] = { 0 }, url[MAX_BUFFER_SIZE] = { 0 }, http[MAX_BUFFER_SIZE] = { 0 };

	if (strlen(jump_http) != 0){
		_snprintf(jump_url, MAX_BUFFER_SIZE, "%s/Logout.do?edubas=%s", jump_http, edubas);
		Config.m_csWebLogoutUrl = jump_url;
		_snprintf(url, MAX_BUFFER_SIZE, "%s&eduuser=%s", jump_url, ip);
		memset(jump_url, 0, MAX_BUFFER_SIZE);
	}
	else
		_snprintf(url, MAX_BUFFER_SIZE, "%s&eduuser=%s", base_url, ip);
	//ip = NULL;//测试用
	int ret = 0;
	ret= DownLoadFileToBuffer(http, MAX_BUFFER_SIZE, url, NULL, timeout, NULL, NULL, jump_url, NULL);
	if (ret > 0)
	{
		if (strstr(jump_url, "/logoutsuccess.jsp") != NULL)
			return "天翼认证已下网成功";
		else if (strstr(jump_url, "/logoutfailed.jsp") != NULL){
			char Getmsg[MAX_BUFFER_SIZE];
			if (!GetHTTPMsg(http, MAX_BUFFER_SIZE, 1, "alertInfo = \"", NULL, NULL, Getmsg, NULL, NULL, NULL, "\""))
			{
				if (!GetHTTPMsg(http, MAX_BUFFER_SIZE, 1, "<div id=\"failed\">", NULL, NULL, Getmsg, NULL, NULL, NULL, "</div>"))
					return "无法获取信息！";
			}
			UTF8ToGBK((unsigned char*)Getmsg, (unsigned char*)Getmsg, MAX_BUFFER_SIZE);
			strncpy(msg, "天翼认证下线失败，服务器信息：", MAX_BUFFER_SIZE);
			strcat(msg, Getmsg);
		}
		else{
			strncpy(msg, "网页地址被重定向到", MAX_BUFFER_SIZE);
			strcat(msg, jump_url);
		}
	}
	else
		sprintf(msg, "天翼认证下线失败,错误代码：%d", ret);
	return msg;
}