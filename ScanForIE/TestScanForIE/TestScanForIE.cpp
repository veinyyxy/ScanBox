// TestScanForIE.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <Psapi.h>

typedef FARPROC (*DllUnregisterServer)();

BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath)
{
	TCHAR szDriveStr[500] = { 0 };
	TCHAR szDrive[3] = { 0 };
	TCHAR szDevName[100] = { 0 };
	INT cchDevName = 0;
	INT i = 0;

	//检查参数
	if (!pszDosPath || !pszNtPath)
		return FALSE;

	//获取本地磁盘字符串
	if (GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for (i = 0; szDriveStr[i]; i += 4)
		{
			if (!lstrcmpi(&(szDriveStr[i]), L"A:\\") || !lstrcmpi(&(szDriveStr[i]), L"B:\\"))
				continue;

			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			szDrive[2] = '\0';
			if (!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名
				return FALSE;

			cchDevName = lstrlen(szDevName);
			if (_wcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//命中
			{
				lstrcpy(pszNtPath, szDrive);//复制驱动器
				lstrcat(pszNtPath, pszDosPath + cchDevName);//复制路径

				return TRUE;
			}
		}
	}

	lstrcpy(pszNtPath, pszDosPath);

	return FALSE;
}

BOOL GetProcessFullPath(DWORD dwPID, TCHAR pszFullPath[MAX_PATH])
{
	TCHAR szImagePath[MAX_PATH];
	HANDLE hProcess;
	if (!pszFullPath)
		return FALSE;

	pszFullPath[0] = '\0';
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, dwPID);
	if (!hProcess)
		return FALSE;

	if (!GetProcessImageFileName(hProcess, szImagePath, MAX_PATH))
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	if (!DosPathToNtPath(szImagePath, pszFullPath))
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	CloseHandle(hProcess);

	wprintf(L"%d,%s \r\n", dwPID, pszFullPath);
	return TRUE;
}

int main()
{
	/*HMODULE hm = LoadLibrary(L".//ScanForIE.dll");

	DllUnregisterServer pDUS = (DllUnregisterServer)GetProcAddress(hm, "DllUnregisterServer");
	FARPROC result = pDUS();
	*/

	DWORD processId = GetCurrentProcessId();
	TCHAR path[MAX_PATH] = { 0 };
	//缓冲区清0
	ZeroMemory(path, MAX_PATH);
	GetProcessFullPath(processId, path);

	PTCHAR first = wcsrchr(path, L"\\");
	wprintf(L"Process ID [ %d ] - [ %s ]\n", processId, path);
	return 0;

	DWORD dwProcessID[4280] = { 0 };
	// 接收总共保存了多少字节到数组
	DWORD dwCBNeed = 0;
	//枚举进程
	EnumProcesses(dwProcessID, sizeof(dwProcessID), &dwCBNeed);
	// 换算成进程的个数
	DWORD dwProcessCnt = dwCBNeed / sizeof(DWORD);
	//循环遍历进程，查找计算器进程
	for (DWORD i = 0; i < dwProcessCnt; i++)
	{
		// 打开所有进程
		//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID[i]);
		//缓冲区
		TCHAR path[MAX_PATH] = { 0 };
		//缓冲区清0
		ZeroMemory(path, MAX_PATH);
		// 获取进程的执行文件名称
		//GetProcessImageFileName(hProcess, path, MAX_PATH);
		GetProcessFullPath(dwProcessID[i], path);
		wprintf(L"Process ID [ %d ] - [ %s ]\n", dwProcessID[i], path);
		//path = c:\\Windows\systemew\calc.exe
	}
}


