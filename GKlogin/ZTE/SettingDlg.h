#if !defined(AFX_SETTINGDLG_H__23034AFF_1612_43AD_B47F_42C19FC2810A__INCLUDED_)
#define AFX_SETTINGDLG_H__23034AFF_1612_43AD_B47F_42C19FC2810A__INCLUDED_

#define URL_ChangeTYPassword "http://125.88.59.149:32003/ENSelfService"
#define URL_Help "http://report.gxgk.cc"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingDlg dialog
#include "Config.h"
#include "HyperLink.h"
#include "Resource.h"
#include "GKloginDlg.h"
#include "afxwin.h"

extern CConfig Config;

class CSettingDlg : public CDialog
{
// Construction
public:
	CSettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingDlg)
	enum { IDD = IDD_SETTING };	
	CHyperLink	m_url;
	CHyperLink	m_author_url;
	CHyperLink	m_modify_url;
	CHyperLink	m_ChangePassword_url;
	//}}AFX_DATA

	CWnd *parent;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	RECT			m_rc;					//存放着窗体的矩形区域

	//线程


	// Generated message map functions
	//{{AFX_MSG(CSettingDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChkWebAuth();
	afx_msg void OnChkWebLogout();
	afx_msg void OnChkEnableWebaccount();		
	afx_msg void OnBtnAutoUpdate();
	afx_msg void OnBtnAutoWinPcapUpdate();
	afx_msg void OnChkHttpHeart();

	//线程函数
	static DWORD WINAPI CheckUpdate(void *para);
	static DWORD WifiDownloads(void *para);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedChangetypassword();
	afx_msg void OnBnClickedChangepassword();
	
	afx_msg void OnEnChangeReauthtime();
	afx_msg void OnBnClickedChkbtimingreauth();
	afx_msg void OnBnClickedAdvanced();
	afx_msg void OnBnClickedAboutUS();
	afx_msg void OnBnClickedChangpwdButton();
	afx_msg void OnBnClickedWifiDown();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGDLG_H__23034AFF_1612_43AD_B47F_42C19FC2810A__INCLUDED_)
