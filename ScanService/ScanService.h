#pragma once
#include <windows.h>
#include <aclapi.h>
#include <tchar.h>
#include <strsafe.h>
#include <QString>
//#include "sample.h"

VOID SvcInstall(void);
VOID DoDeleteSvc();
VOID WINAPI SvcCtrlHandler(DWORD);
VOID WINAPI SvcMain(DWORD, LPWSTR *);
VOID WINAPI DisplayUsage(void);

VOID WINAPI DoStartSvc(void);
VOID WINAPI DoUpdateSvcDacl(void);
VOID WINAPI DoStopSvc(void);
BOOL WINAPI StopDependentServices(void);
VOID DoAutoStartSvc();
VOID DoDemandStartSvc();
VOID ReportSvcStatus(DWORD, DWORD, DWORD);
VOID SvcInit(int iArgc, LPWSTR* cArgv);
VOID SvcReportEvent(LPTSTR, WORD = EVENTLOG_ERROR_TYPE);

BOOL EnableDebugPrivilege();
int ModifyProcessToken(HANDLE processHandle);
BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath);
DWORD GetProcessFullPath(DWORD dwPID, TCHAR pszFullPath[MAX_PATH]);
DWORD CheckMainServerProcess(const QString& strScanFacdeName);
int GetScanFacdeName(QString& strScanFacdeName);
DWORD LaunchMainServerProcess();
//DWORD LaunchMainServerProcess(const QString& strScanFacdeName);
BOOL TerminateServerProcess();
DWORD LaunchMainServerProcessAsUser();
bool AdjustProcessTokenPrivilege();
DWORD SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnable = TRUE);
VOID ElevationPrivilege();