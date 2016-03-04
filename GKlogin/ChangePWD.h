#pragma once
#include "Resource.h"
#include "Define.h"
#include "Config.h"
#include "WebAuth.h"
#include "GKloginDlg.h"
#include "AutoUpdate.h"

#define MAX_BUFFER_SIZE 102040
class CChangePWD: public CDialogEx
{
public:
	CChangePWD(CWnd* pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_ChangePWD };
	virtual BOOL OnInitDialog();
protected:
	char Account[MAX_STRING]; 
	char PassWordOld[MAX_STRING];
	char PassWordNew1[MAX_STRING];
	char PassWordNew2[MAX_STRING];
	int WebChangePassWord();
	int ZTEChangePassWord();
	virtual void OnOK();
	char * PostChangeMessages(char *http);
public:
};

