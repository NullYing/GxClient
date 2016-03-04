
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define NO_WARN_MBCS_MFC_DEPRECATION  

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#include <afxdisp.h>        // MFC 自动化类

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持
//Curl
#define CURL_STATICLIB
#include "curl/curl.h"
#pragma comment(lib, "Wldap32.lib")//curl依赖库
#ifdef _DEBUG
#pragma comment(lib, "libcurld.lib")
#else
#pragma comment(lib, "libcurl.lib")

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
//#pragma comment(lib, "libcurld_static.lib")//VS2013生成，不支持XP
#endif
//Winsock2
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
//WinPcap
#include "pcap.h"
#pragma comment(lib, "wpcap.lib")
//Iphlpapi
#include <Iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib")

//调用测试用控制台
//#pragma comment(linker, "/subsystem:console /entry:WinMainCRTStartup")

#pragma comment(lib, "libpng16.lib")
#pragma comment(lib, "zlib.lib")  
#pragma warning (default: 4800)

#pragma comment(lib, "Version.lib")  

typedef  unsigned char	u_char;


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


