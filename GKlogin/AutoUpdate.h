#ifndef AUTOUPDATE_H
#define AUTOUPDATE_H

#define WEB_URL    TEXT("http://zte.gxgk.cc/gklogin")
#ifdef _DEBUG
#define LAN_URL  TEXT("http://zte.gxgk.cc/gklogintest")
#else
#define LAN_URL  TEXT("http://ztelan.gxgk.cc/gklogin")
#endif
#define URL_ZTE      TEXT("/ZTE.exe")
#define URL_WinPcap  TEXT("/WinPcap.exe")
#define NoticeUrl    TEXT("/Notice.html")
#define NoticeUp    TEXT("/Notice.php")

#define MAX_STRING 2048
#define DOWNTIMEOUT 10
typedef struct _FileInfo{
	time_t modifytime;
	size_t filesize;
	char Version[MAX_STRING];
	char WinPcapVer[MAX_STRING];
} FileInfo;

typedef struct _buffer{
	char *buffer;
	size_t size;
	int offset;

} Buffer;

size_t write_buffer_data(void *ptr, size_t size, size_t nmemb, void *buffer);
size_t write_file_data(void *ptr, size_t size, size_t nmemb, void *stream);
int AutoUpdate(bool m_SlienceUp);
int AutoUpdateWinPcap();
FileInfo *GetFileModifyTime();
int CompareVersion(char *localver, char *remotever);
FileInfo *GetHttpModifyTime(const char *url);
char *RandomSessionID(const char *url);
int DownLoadFile(const char *savename, const char *url, const char *ip, const int timeout, char *cookiefile, const char *cookiejar);
int DownLoadFileToBuffer(char *buffer, int size, const char *url, const char *ip, const int timeout, const char *cookies, const char *cookiesfile, char * redirectUrl, long * retcode);
int SendPost(char *buffer, int size, const char *url, const char *ip, const int timeout, const char *cookies, const char *cookiefile, const char *cookiejar, const char *data);
int VersionUpdate(FileInfo *fileinfo);
int GetHTTPMsg(const char  *http, const int size,const int num, char sign1[], const char sign2[], const char  sign3[], char *data1, char *data2, char *data3, const char begain[], const char endsign[]);
int UTF8ToGBK(unsigned char * lpUTF8Str, unsigned char * lpGBKStr, int nGBKStrLen);
int GBKToUTF8(unsigned char * lpGBKStr, unsigned char * lpUTF8Str, int nUTF8StrLen);
CString URLEncode(CString sIn);
#endif