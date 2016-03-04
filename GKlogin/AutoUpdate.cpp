#include "stdafx.h"
#include "Define.h"
#include "AutoUpdate.h"
#include <stdio.h>
#include "time.h"
#define FILETIME_LENGTH	64

static size_t write_file_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

 size_t write_buffer_data(void *ptr, size_t size, size_t nmemb, void *buffer)
{
	Buffer *buf = (Buffer*)buffer;
	if(buf->buffer == NULL) return size;
	if(buf->offset + size >= buf->size) return 0;
	memcpy(buf->buffer + buf->offset, ptr, size * nmemb);	
	buf->offset += size * nmemb;
	return size * nmemb;
}

FileInfo *GetFileModifyTime(const char* url)
{
	static FileInfo info;
	char filepath[MAX_PATH] = {0};
	info.filesize = 0; info.modifytime = 0;

	if(!GetModuleFileName(NULL, filepath, MAX_PATH)) return NULL;

	HANDLE hFile = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);
	if(hFile == NULL) return NULL;

	FILETIME LastModifyTime;

	LARGE_INTEGER largeInt;
	if(GetFileTime(hFile, NULL, NULL, &LastModifyTime)) {		
		largeInt.QuadPart =  ((DWORD64)LastModifyTime.dwHighDateTime << 32) + LastModifyTime.dwLowDateTime;
		info.modifytime = (long)((largeInt.QuadPart - 116444736000000000) /  10000000);
	}

	info.filesize = GetFileSize(hFile, NULL)/1024;
	strcpy(info.Version , STR_Version);
	CloseHandle(hFile);
	
	return &info;
}

int AutoUpdate(bool m_SlienceUp)
{
	FileInfo *remote_info = GetHttpModifyTime(LAN_URL URL_ZTE);
	if (remote_info == NULL){
		remote_info = GetHttpModifyTime(WEB_URL URL_ZTE);
	}
	FileInfo *local_info = GetFileModifyTime(NULL);
	if (local_info == NULL || remote_info == NULL)
	{
		//AfxMessageBox("获取本地信息失败或远程服务器无法连接，检测更新失败，请手动更新！",  MB_TOPMOST);
		return -1;
	}
	if (CompareVersion(local_info->Version, remote_info->Version))
	{
		char msg[1024] = "发现新版本，立即进行更新吗？\r\n\r\n";
		char szTime[1024];
		strftime(szTime, 1024, "%Y-%m-%d %H:%M:%S", localtime(&remote_info->modifytime));
		strcat(msg, "新版本信息:\r\n文件时间："); strcat(msg, szTime); strcat(msg, "\r\n文件大小：");
		_itoa(remote_info->filesize, szTime, 10); strcat(msg, szTime); strcat(msg, "KB\r\n");
		strcat(msg, "版本号：");
		strcat(msg, remote_info->Version); strcat(msg, "\r\n\r\n");

		strftime(szTime, 1024, "%Y-%m-%d %H:%M:%S", localtime(&local_info->modifytime));
		strcat(msg, "当前版本信息:\r\n文件时间："); strcat(msg, szTime); strcat(msg, "\r\n文件大小：");
		_itoa(local_info->filesize, szTime, 10); strcat(msg, szTime); strcat(msg, "KB");
		strcat(msg, "\r\n版本号：");
		strcat(msg, local_info->Version); strcat(msg, "\r\n");
		if (!m_SlienceUp){
			if (IDYES != AfxMessageBox(msg, MB_YESNO | MB_ICONINFORMATION | MB_TOPMOST| MB_TOPMOST)) return 1;
		}
		char szTempPath[MAX_PATH], szTempName[MAX_PATH];
		GetTempPath(MAX_PATH, szTempPath);
		GetTempFileName(szTempPath, _T("ZTE"), 0, szTempName);
		strcat(szTempName, ".exe");

		if (DownLoadFile(szTempName, LAN_URL URL_ZTE, NULL, DOWNTIMEOUT, NULL, NULL) != 0)
		{
			if (DownLoadFile(szTempName, WEB_URL URL_ZTE, NULL, DOWNTIMEOUT*3, NULL, NULL) != 0)
			{
				//AfxMessageBox("下载客户端失败，请手动更新！",  MB_TOPMOST);
				return -2;
			}
		}
		if (!m_SlienceUp){
			if (IDOK != AfxMessageBox("更新认证客户端期间可能短暂无法使用网络，请备份好数据，然后按“确定”开始更新。",
				MB_OK | MB_ICONINFORMATION |  MB_TOPMOST)) {
				return 2;
			}
		}
		char filepath[MAX_PATH] = { 0 };
		char parameters[MAX_PATH] = "-update \"";
		if (!GetModuleFileName(NULL, filepath, MAX_PATH)) return -1;
		strcat(parameters, filepath);

		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.lpFile = szTempName;
		ShExecInfo.lpParameters = parameters;
		ShExecInfo.nShow = SW_HIDE;

		if (!ShellExecuteEx(&ShExecInfo)) {
			AfxMessageBox(_T("启动更新程序失败！"), MB_TOPMOST);
			DeleteFile(szTempName);
			return 3;
		}
		Sleep(1000);

		return 0;
	}
	//UpdateFile(URL_ZTE, GetFileModifyTime());
	
	return 4;
}

int AutoUpdateWinPcap()
{
	FileInfo *remote_info = GetHttpModifyTime(LAN_URL URL_WinPcap);
	if (remote_info == NULL)
	{
		remote_info = GetHttpModifyTime(WEB_URL URL_WinPcap);
		if (remote_info == NULL)
		{
			//AfxMessageBox("远程服务器无法连接，请手动更新WinPcap！",  MB_TOPMOST);
			return -1;
		}
	}
	CString LocalWinPcapinfo = const_cast<char*>(pcap_lib_version());
	LocalWinPcapinfo = LocalWinPcapinfo.Mid(16, 5);

	char LocalVer[20];
	strcpy(LocalVer, LocalWinPcapinfo);
	if (CompareVersion(LocalVer, remote_info->WinPcapVer))
	{
		char msg[1024] = "发现WinPcap新版本，立即进行更新WinPcap吗？\r\n\r\n";
		char szTime[1024];
		strftime(szTime, 1024, "%Y-%m-%d %H:%M:%S", localtime(&remote_info->modifytime));
		strcat(msg, "新版本信息:\r\n文件时间："); strcat(msg, szTime); strcat(msg, "\r\n文件大小：");
		_itoa(remote_info->filesize, szTime, 10); strcat(msg, szTime); strcat(msg, "KB");
		strcat(msg, "\r\n版本号：");
		strcat(msg, remote_info->WinPcapVer); strcat(msg, "\r\n\r\n");

		strcat(msg, "当前版本信息:\r\n"); strcat(msg, "版本号：");
		strcat(msg, LocalWinPcapinfo); strcat(msg, "\r\n");
		
		if (IDYES != AfxMessageBox(msg, MB_YESNO | MB_ICONINFORMATION| MB_TOPMOST)) return 1;
		if (DownLoadFile(".\\WinPcap.exe", LAN_URL URL_WinPcap, NULL, DOWNTIMEOUT, NULL, NULL) != 0)
		{
			if (DownLoadFile(".\\WinPcap.exe", WEB_URL URL_WinPcap, NULL, DOWNTIMEOUT * 3, NULL, NULL) != 0)
			{
				AfxMessageBox(_T("下载WinPcap失败，请手动更新！"), MB_TOPMOST);
				return -2;
			}
		}
		if (IDOK != AfxMessageBox(_T("更新WinPcap期间可能短暂无法使用网络，请按“确定”开始更新。"), MB_TOPMOST| MB_OK | MB_ICONINFORMATION)) {
			return 2;
		}

		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.lpFile = ".\\WinPcap.exe";
		ShExecInfo.nShow = SW_HIDE;

		if (!ShellExecuteEx(&ShExecInfo)) {
			AfxMessageBox(_T("启动更新程序失败！"),  MB_TOPMOST);
			return 3;
		}
		Sleep(1000);
		return 0;
	}
	return 4;
}
int DownLoadFile(const char *savename, const char *url, const char *ip, const int timeout, char *cookiefile, const char *cookiejar)
{
	FILE *file = NULL;
	file = fopen(savename, "wb");
	if(file == NULL) return 1;

	CURL *curl;
	CURLcode res;
	long retcode = 0;
				
	curl = curl_easy_init();
	if(curl) {	
		curl_easy_setopt(curl, CURLOPT_URL, url);		
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		if(ip) curl_easy_setopt(curl, CURLOPT_INTERFACE, ip);
		if(timeout > 0) curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		if (cookiefile)curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiefile);//保存cookies
		else curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		if (cookiejar)curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiejar);//读取cookies
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//认证之前会自动转跳
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)file);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2486.0 Safari/537.36");
		res = curl_easy_perform(curl);
		if (res != CURLE_OK && res != CURLE_WRITE_ERROR) {
			if (res == CURLE_OPERATION_TIMEDOUT)
			{
				fclose(file);
				file = NULL;
				DeleteFile(savename);
				curl_easy_cleanup(curl);
				return -1;
			}
			fclose(file);
			file = NULL;
			DeleteFile(savename);
			curl_easy_cleanup(curl);
			return -2;
		}

		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if (res != CURLE_OK || retcode != 200) {
			fclose(file);
			file = NULL;
			DeleteFile(savename);
			curl_easy_cleanup(curl);
			return -3;
		}
		
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	fclose(file);
	file = NULL;
	return 0;
}

int DownLoadFileToBuffer(char *buffer, int size, const char *url, const char *ip, const int timeout, const char *cookies, const char *cookiesfile, char * redirectUrl, long * retcode)
{
	CURL *curl;
	CURLcode res;
	if (retcode)
		*retcode = 0;
	else{
		long retcodenum;
		retcode = &retcodenum;
	}
	Buffer buf;

	buf.buffer = buffer;
	buf.size = size;
	buf.offset = 0;
	
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_REFERER, url);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);			//打印调试信息
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
		if (cookiesfile)curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiesfile);
		else curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		if(cookies)  curl_easy_setopt(curl, CURLOPT_COOKIE, cookies); 		
		if(ip) curl_easy_setopt(curl, CURLOPT_INTERFACE, ip);
		if(timeout > 0) curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2486.0 Safari/537.36");
		if (buffer){
			curl_easy_setopt(curl, CURLOPT_NOBODY, 0);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);//认证之前会自动转跳
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer_data);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		}
		else{
			curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0);
		}
		res = curl_easy_perform(curl);
		if (res != CURLE_OK){
			switch (res){
			case CURLE_COULDNT_RESOLVE_HOST:{//无法解析主机。给定的远程主机没有得到解决。
				curl_easy_cleanup(curl);
				return -1;
			}
			case CURLE_COULDNT_CONNECT:{//不能连接到remote 主机或者代理
				curl_easy_cleanup(curl);
				return -2;
			}
			case CURLE_WRITE_ERROR:{
				curl_easy_cleanup(curl);
				return -3;
			}
			case CURLE_OPERATION_TIMEDOUT:{
				curl_easy_cleanup(curl);
				return -4;
			}
			default:{
				curl_easy_cleanup(curl);
				return -5;
			}
			}
		}
		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , retcode); 
		if (redirectUrl != NULL){
			char *r = NULL;
			long lenght = 2048;
			res = curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &r);
			if (r != NULL)
				strncpy(redirectUrl, r, lenght);
			else
			{
				res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &r);
				if (r != NULL)
					strncpy(redirectUrl, r,lenght);
			}
		}
		if(res != CURLE_OK || *retcode != 200) {
			curl_easy_cleanup(curl);
			return -10;
		}

		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	//printf("\n%s",buffer);
	return buf.offset;
}

FileInfo *GetHttpModifyTime(const char* url)
{
	
	static FileInfo fileinfo;
	if (VersionUpdate(&fileinfo) == NULL)
		return NULL;
	double size;
	CURL *curl;
	CURLcode res;
	long retcode = 0;
	fileinfo.filesize = 0; fileinfo.modifytime = 0;
			
	curl = curl_easy_init();
	if(curl) {	
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_FILETIME, 1);		
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//认证之前会自动转跳
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2486.0 Safari/537.36");
		res = curl_easy_perform(curl);
		if(res == CURLE_OK) {	
			res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
			if(res == CURLE_OK && retcode == 200) {				
				res = curl_easy_getinfo(curl, CURLINFO_FILETIME, &fileinfo.modifytime);				
				if(res != CURLE_OK) fileinfo.modifytime = 0;
				res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);
				if(res != CURLE_OK) fileinfo.filesize = 0;
				else fileinfo.filesize = (int)size;
				curl_easy_cleanup(curl);
				fileinfo.filesize = fileinfo.filesize / 1024;
				return &fileinfo;
			}
		}
		/* always cleanup */ 
		curl_easy_cleanup(curl);
		
	}
	
	return NULL;
}
int VersionUpdate(FileInfo *fileinfo)
{
	char http[MAX_STRING] = { 0 };
	int ret = DownLoadFileToBuffer(http, MAX_STRING, LAN_URL NoticeUrl, NULL, DOWNTIMEOUT, NULL, NULL,NULL,NULL);
	if (ret <= 0)
	{
		ret = DownLoadFileToBuffer(http, MAX_STRING, WEB_URL NoticeUrl, NULL, DOWNTIMEOUT, NULL, NULL, NULL,NULL);
		if (ret <= 0)
		{
			return NULL;
		}
	}
	GetHTTPMsg(http, MAX_STRING, 2, "WinPcap:\"", "Version:\"", NULL, fileinfo->WinPcapVer, fileinfo->Version, NULL, NULL, "\"");

	if (strlen(fileinfo->Version) == 0 && strlen(fileinfo->WinPcapVer) == 0){
		return NULL;
	}
	return 1;
}

size_t write_data(void* buffer, size_t size, size_t nmemb, void *stream)
{
	    FILE *fptr = (FILE*)stream;
	    fwrite(buffer, size, nmemb, fptr);
	    return size*nmemb;
}

int SendPost(char *buffer, int size, const char *url, const char *ip, const int timeout, const char *cookies, const char *cookiefile, const char *cookiejar, const char *data)
{
	CURL *curl;
	CURLcode res;
	long retcode = 0;
	Buffer buf;
	buf.buffer = buffer;
	buf.size = size;
	buf.offset = 0;

	struct curl_slist *http_header = NULL;
	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url); //url地址
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data); //post参数

		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer_data);//对返回的数据进行操作的函数地址
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);//这是write_data的第四个参数值
		curl_easy_setopt(curl, CURLOPT_POST, 1); //设置问非0表示本次操作为post
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //打印调试信息
		//curl_easy_setopt(curl, CURLOPT_NOBODY, 0);
		curl_easy_setopt(curl, CURLOPT_HEADER, 0); //将响应头信息和相应体一起传给write_data
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); //设置为非0,响应头信息location, 设置支持302重定向 
		if (cookiefile)curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiefile);//保存cookies
		else curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		if (cookiejar)curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiejar);//读取cookies
		if (cookies)  curl_easy_setopt(curl, CURLOPT_COOKIE, cookies);

		//curl_easy_setopt(curl, CURLOPT_REFERER, url);//设置header中"Referer: " 部分的值。			
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L); //进度条
		if (ip) curl_easy_setopt(curl, CURLOPT_INTERFACE, ip);//在外部网络接口中使用的名称，可以是一个接口名，IP或者主机名。
		if (timeout > 0) curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2486.0 Safari/537.36");

		res = curl_easy_perform(curl);

		if (res != CURLE_OK && res != CURLE_WRITE_ERROR) {
			if (res == CURLE_OPERATION_TIMEDOUT)
			{
				curl_easy_cleanup(curl);
				return -1;
			}
			curl_easy_cleanup(curl);
			return -2;
		}
		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if (res != CURLE_OK || retcode != 200) {
			curl_easy_cleanup(curl);
			return -3;
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	return buf.offset;
}
int GetHTTPMsg(const char  *http,const int size, const int num, char sign1[], const char sign2[], const char  sign3[], char *data1, char *data2, char *data3, const char begainsign[], const char endsign[])
{
	char *x = (char *)malloc(size), *q, *p, *w;
	p = x;
	char Sign[MAX_STRING];
	int SeccessSign = 1;
	strncpy(p, http, size);
	for (int i = 1; i <= num; i++)
	{
		p = x;
		strcpy(p, http);//strcpy会移动指针，必须重新赋值
		if (i == 1)strncpy(Sign, sign1 ,MAX_STRING);
		if (i == 2)strncpy(Sign, sign2, MAX_STRING);
		if (i == 3)strncpy(Sign, sign3, MAX_STRING);
		p = strstr(p, Sign);
		if (p == NULL)
		{
			printf("失败1");
			SeccessSign = 0;
			continue;
		}
		p += strlen(Sign);
		if (begainsign != NULL){
			p = strstr(p, begainsign);
			if (p == NULL)
			{
				printf("失败2");
				SeccessSign = 0;
				continue;
			}
			p += strlen(begainsign);
		}
		if (*p == '\r'){
			p += 2;
			for (; *p == '\t';p++);
		}
		
		q = strstr(p, endsign);
		if (q == NULL)
		{
			printf("失败3");
			SeccessSign = 0;
			continue;
		}
		*q = '\0';
		w = strstr(p,"\r\n");
		if (w != NULL)*w = '\0';
		if (i == 1)strcpy(data1, p);
		if (i == 2)strcpy(data2, p);
		if (i == 3)strcpy(data3, p);
	}
	free(x);
	return SeccessSign;
}
int CompareVersion(char *localver,char *remotever){
	char LocalVer[20], RomoteVer[20], TempVar[20]; int a = 0;
	for (int b = 1; b <= 2; b++)
	{
		if (b == 1)strncpy(TempVar, localver, strlen(localver));
		if (b == 2)strncpy(TempVar, remotever, strlen(remotever));

		for (int i = 0; TempVar[i] != '\0'; i++)
		{
			if (TempVar[i] == '.'){
				a = i;
				for (; TempVar[i] != '\0'; i++){
					TempVar[i] = TempVar[i + 1];
				}
				i = a;
			}
		}
		if (b == 1)strncpy(LocalVer, TempVar, strlen(localver));
		if (b == 2)strncpy(RomoteVer, TempVar, strlen(remotever));
	}
	if (atoll(LocalVer) < atoll(RomoteVer))
		return 1;
	else
		return 0;
}
int UTF8ToGBK(unsigned char * lpUTF8Str, unsigned char * lpGBKStr, int nGBKStrLen)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if (!lpUTF8Str) //如果UTF8字符串为NULL则出错退出
		return 0;

	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)lpUTF8Str, -1, NULL, NULL); //获取转换到Unicode编码后所需要的字符空间长度
	lpUnicodeStr = new WCHAR[nRetLen + 1]; //为Unicode字符串空间
	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)lpUTF8Str, -1, lpUnicodeStr, nRetLen); //转换到Unicode编码
	if (!nRetLen) //转换失败则出错退出
		return 0;

	nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, NULL, NULL, NULL, NULL); //获取转换到GBK编码后所需要的字符空间长度

	if (!lpGBKStr) //输出缓冲区为空则返回转换后需要的空间大小
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return nRetLen;
	}

	if (nGBKStrLen < nRetLen) //如果输出缓冲区长度不够则退出
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return 0;
	}

	nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, (char *)lpGBKStr, nRetLen, NULL, NULL); //转换到GBK编码

	if (lpUnicodeStr)
		delete[]lpUnicodeStr;

	return nRetLen;
}
inline BYTE toHex(const BYTE &x)
{
	return x > 9 ? x + 55 : x + 48;
}
CString URLEncode(CString sIn)
{
	CString sOut;
	const int nLen = sIn.GetLength() + 1;
	register LPBYTE pOutTmp = NULL;
	LPBYTE pOutBuf = NULL;
	register LPBYTE pInTmp = NULL;
	LPBYTE pInBuf = (LPBYTE)sIn.GetBuffer(nLen);
	BYTE b = 0;

	//alloc out buffer
	pOutBuf = (LPBYTE)sOut.GetBuffer(nLen * 3 - 2);//new BYTE [nLen * 3];

	if (pOutBuf)
	{
		pInTmp = pInBuf;
		pOutTmp = pOutBuf;

		// do encoding
		while (*pInTmp)
		{
			if (isalnum(*pInTmp))
				*pOutTmp++ = *pInTmp;
			else
				if (isspace(*pInTmp))
					*pOutTmp++ = '+';
				else
				{
					*pOutTmp++ = '%';
					*pOutTmp++ = toHex(*pInTmp >> 4);
					*pOutTmp++ = toHex(*pInTmp % 16);
				}
			pInTmp++;
		}
		*pOutTmp = '\0';
		//sOut=pOutBuf;
		//delete [] pOutBuf;
		sOut.ReleaseBuffer();
	}
	sIn.ReleaseBuffer();
	return sOut;
}