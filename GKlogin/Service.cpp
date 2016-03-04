#include "StdAfx.h"
#include "GKloginDlg.h"
#include "Winsvc.h"

DWORD CGKloginDlg::DetectServiceStats(LPCTSTR pszName)
{
	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	if (schSCManager == NULL)
	{
		return -1;
	}
	SC_HANDLE schService = ::OpenService(schSCManager, pszName, SERVICE_QUERY_STATUS);
	if (schService == NULL)
	{
		::CloseServiceHandle(schSCManager);
		return -1;
	}
	SERVICE_STATUS ssStatus;
	BOOL bRet = ::QueryServiceStatus(schService, &ssStatus);
	::CloseServiceHandle(schService);
	::CloseServiceHandle(schSCManager);
	if (bRet) return ssStatus.dwCurrentState;
	return -1;
}

DWORD CGKloginDlg::StopService(LPCTSTR pszName)
{
	if (DetectServiceStats(pszName) == SERVICE_STOPPED)
	{
		ASSERT(0);
		return 0;
	}
	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager == NULL)
	{
		ASSERT(0);
		return -1;
	}
	SC_HANDLE schService = ::OpenService(schSCManager, pszName, SERVICE_ALL_ACCESS);
	if (schService == NULL)
	{
		ASSERT(0);
		::CloseServiceHandle(schSCManager);
		return -1;
	}
	SERVICE_STATUS ssStatus;
	BOOL bRet = ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus);
	if (bRet)
	{
		if (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
		{
			Sleep(1000);
			while (::QueryServiceStatus(schService, &ssStatus))
			{
				if (ssStatus.dwCurrentState == SERVICE_START_PENDING) Sleep(1000);
				else break;
			}
		}
		if (ssStatus.dwCurrentState != SERVICE_STOPPED) bRet = FALSE;
	}
	else
	{
		ASSERT(0);
	}
	::CloseServiceHandle(schService);
	::CloseServiceHandle(schSCManager);
	if (bRet) return ssStatus.dwCurrentState;
	ASSERT(0);
	return -1;
}

DWORD CGKloginDlg::StartServiceA(LPCTSTR pszName)
{
	if (DetectServiceStats(pszName) == SERVICE_RUNNING) return 0;
	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager == NULL) return -1;
	SC_HANDLE schService = ::OpenService(schSCManager, pszName, SERVICE_ALL_ACCESS);
	if (schService == NULL)
	{
		::CloseServiceHandle(schSCManager);
		return -1;
	}
	SERVICE_STATUS ssStatus;
	BOOL bRet = ::StartService(schService, 0, 0);
	if (bRet)
	{
		Sleep(1000);
		while (::QueryServiceStatus(schService, &ssStatus))
		{
			if (ssStatus.dwCurrentState == SERVICE_START_PENDING) Sleep(1000);
			else break;
		}
		if (ssStatus.dwCurrentState != SERVICE_RUNNING) bRet = FALSE;
	}
	::CloseServiceHandle(schService);
	::CloseServiceHandle(schSCManager);
	if (bRet) return ssStatus.dwCurrentState;
	return -1;
}