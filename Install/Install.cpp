// Install.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <strsafe.h>
#include <msi.h>
#include <Msiquery.h>
const char* g_ChromeHostJson = "{\"name\": \"veiny.scanbox.forchrome\",\"description\" : \"ScanBox\",\"path\" : \"%sScanBoxChromeExtension.exe\",\"type\" : \"stdio\",\"allowed_origins\" : [\"chrome-extension://%s/\"]}";

void writeLogingW(LPCTSTR fileName, LPCTSTR strFormat, ...)
{
	FILE *fp = NULL;
#ifdef  UNICODE
	_wfopen_s(&fp, fileName, TEXT("a"));
#else
	fopen_s(&fp, fileName, TEXT("a"));
#endif //  UNICODE
	///////////////////////////////////////////////////
	int _Result;
	va_list _ArgList;
	va_start(_ArgList, strFormat);

#ifdef  UNICODE
	_Result = _vfwprintf_s_l(fp, strFormat, NULL, _ArgList);
#else
	_Result = _vfprintf_s_l(fp, strFormat, NULL, _ArgList);
#endif //  UNICODE
	
	va_end(_ArgList);
	//return _Result;
	///////////////////////////////////////////////////
	fclose(fp);
}

void writeLogingA(const char* fileName, const char* strFormat, ...)
{
	FILE *fp = NULL;
	fopen_s(&fp, fileName, "a");
	///////////////////////////////////////////////////
	int _Result;
	va_list _ArgList;
	va_start(_ArgList, strFormat);
	_Result = _vfprintf_s_l(fp, strFormat, NULL, _ArgList);
	va_end(_ArgList);
	fclose(fp);
}

char* strrpcc(char* str, const char* oldstr, const char* newstr)
{
	size_t bufSize = strlen(str) + 128;
	char* bstr = new char[bufSize]; //转换缓冲区
	memset(bstr, 0, bufSize);

	for (size_t i = 0; i < strlen(str); i++)
	{
		if (!_strnicmp(str + i, oldstr, strlen(oldstr))) //查找目标字符串
		{
			strcat_s(bstr, bufSize, newstr);
			i += strlen(oldstr) - 1;
		}
		else
		{
			strncat_s(bstr, bufSize, str + i, 1); //保存一字节进缓冲区
		}
	}
	strcpy_s(str, bufSize, bstr);

	delete[] bstr;
	return str;
}

LPWSTR strrpc(LPWSTR str, LPCWCH oldstr, LPCWCH newstr)
{
	size_t bufSize = wcslen(str) + 128;
	TCHAR* bstr = new TCHAR[bufSize]; //转换缓冲区
	memset(bstr, 0, bufSize);

	for (size_t i = 0; i < wcslen(str); i++)
	{
		if (!_wcsnicmp(str + i, oldstr, wcslen(oldstr))) //查找目标字符串
		{
			wcscat_s(bstr, bufSize, newstr);
			i += wcslen(oldstr) - 1;
		}
		else
		{
			wcsncat_s(bstr, bufSize, str + i, 1); //保存一字节进缓冲区
		}
	}
	wcscpy_s(str, bufSize, bstr);

	delete[] bstr;
	return str;
}

char* wcharToChar(wchar_t* src)
{
	int iSize = 0;
	char* pszMultiByte = 0;

	iSize = WideCharToMultiByte(CP_ACP, 0, src, -1, NULL, 0, NULL, NULL);
	pszMultiByte = (char*)malloc(iSize * sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, src, -1, pszMultiByte, iSize, NULL, NULL);
	return pszMultiByte;
}

UINT getTargetDir(MSIHANDLE hInstall, LPWSTR* szValueBuf, DWORD* pLen)
{
	
	//TCHAR* szValueBuf = NULL;
	DWORD cchValueBuf = 0;
	TCHAR ValueBuf[] = TEXT("");
	UINT uiStat = MsiGetProperty(hInstall, TEXT("CustomActionData"), ValueBuf, &cchValueBuf);
	if (ERROR_MORE_DATA == uiStat)
	{
		++cchValueBuf; // on output does not include terminating null, so add 1
		*szValueBuf = new TCHAR[cchValueBuf];

		*pLen = cchValueBuf;
		if (*szValueBuf)
		{
			uiStat = MsiGetProperty(hInstall, TEXT("CustomActionData"), *szValueBuf, &cchValueBuf);
		}
	}
	if (ERROR_SUCCESS != uiStat)
	{
		if (*szValueBuf != NULL)
		{
			delete[] (*szValueBuf);
			*szValueBuf = NULL;
		}
			
		return ERROR_INSTALL_FAILURE;
	}

	// custom action uses MyProperty
	//delete[] *szValueBuf;
	return ERROR_SUCCESS;

}

int RegChromeHost(const wchar_t* pReg)
{
	// Name of the event log.
	const wchar_t *logName = L"Application";
	// Event Source name.
	const wchar_t *hostName = L"veiny.scanbox.forchrome";
	// DLL that contains the event messages (descriptions).

	// This number of categories for the event source.
	DWORD dwCategoryNum = 1;

	HKEY hk;
	DWORD dwDisp;
	TCHAR szBuf[MAX_PATH] = { 0 };
	TCHAR hostJsonFile[MAX_PATH] = { 0 };
	size_t cchSize = MAX_PATH;

	// Create the event source as a subkey of the log. 
	HRESULT hr = StringCchPrintf(szBuf, cchSize,
		L"Software\\Google\\Chrome\\NativeMessagingHosts\\%s",
		hostName);

	//writeLogingW(L"C:\\Windows\\Temp\\ScanBox_install_log.txt", L"Register KEY : %S\r\n", szBuf);

	hr = StringCchPrintf(hostJsonFile, cchSize,
		L"%sscanboxforchrome_win.json",
		pReg);

	HKEY currentUserKey;
	RegOpenCurrentUser(KEY_WRITE, &currentUserKey);

	if (RegCreateKeyEx(currentUserKey, szBuf,
		0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_WRITE, NULL, &hk, &dwDisp))
	{
		//writeLogingW(L"C:\\Windows\\Temp\\ScanBox_install_log.txt", L"RegCreateKeyEx ERROR : %d\r\n", GetLastError());
		return 0;
	}

	if (RegSetValueEx(hk,             // subkey handle 
		NULL,        // value name 
		0,							// must be zero 
		REG_SZ,						// value type 
		(LPBYTE)hostJsonFile,          // pointer to value data 
		(DWORD)(lstrlen(hostJsonFile) + 1) * sizeof(TCHAR))) // data size
	{
		//writeLogingW(L"C:\\Windows\\Temp\\ScanBox_install_log.txt", L"RegSetValueEx ERROR : %d\r\n", GetLastError());
		RegCloseKey(hk);
		return 0;
	}

	RegCloseKey(hk);
	return 1;
}

BOOL RegDelnodeRecurse(HKEY hKeyRoot, LPTSTR lpSubKey)
{
	LPTSTR lpEnd;
	LONG lResult;
	DWORD dwSize;
	TCHAR szName[MAX_PATH];
	HKEY hKey;
	FILETIME ftWrite;

	// First, see if we can delete the key without having
	// to recurse.

	lResult = RegDeleteKey(hKeyRoot, lpSubKey);

	if (lResult == ERROR_SUCCESS)
		return TRUE;

	lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

	//RegOpenCurrentUser(KEY_ALL_ACCESS, &hKey);

	if (lResult != ERROR_SUCCESS)
	{
		if (lResult == ERROR_FILE_NOT_FOUND) {
			printf("Key not found.\n");
			return TRUE;
		}
		else {
			printf("Error opening key.\n");
			return FALSE;
		}
	}

	// Check for an ending slash and add one if it is missing.

	lpEnd = lpSubKey + lstrlen(lpSubKey);

	if (*(lpEnd - 1) != TEXT('\\'))
	{
		*lpEnd = TEXT('\\');
		lpEnd++;
		*lpEnd = TEXT('\0');
	}

	// Enumerate the keys

	dwSize = MAX_PATH;
	lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
		NULL, NULL, &ftWrite);

	if (lResult == ERROR_SUCCESS)
	{
		do {

			StringCchCopy(lpEnd, MAX_PATH * 2, szName);

			if (!RegDelnodeRecurse(hKeyRoot, lpSubKey)) {
				break;
			}

			dwSize = MAX_PATH;

			lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
				NULL, NULL, &ftWrite);

		} while (lResult == ERROR_SUCCESS);
	}

	lpEnd--;
	*lpEnd = TEXT('\0');

	RegCloseKey(hKey);

	// Try again to delete the key.

	lResult = RegDeleteKey(hKeyRoot, lpSubKey);

	if (lResult == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL RegDelnode(HKEY hKeyRoot, const LPTSTR lpSubKey)
{
	TCHAR szDelKey[2 * MAX_PATH];

	StringCchCopy(szDelKey, MAX_PATH * 2, lpSubKey);
	return RegDelnodeRecurse(hKeyRoot, szDelKey);

}

UINT install(MSIHANDLE hInstall)
{
	//return ERROR_INSTALL_FAILURE;
	UINT uiStat = NULL;
	LPWSTR pTargetDir = NULL;
	DWORD dwLen = 0;

	uiStat = getTargetDir(hInstall, &pTargetDir, &dwLen);

	if (ERROR_SUCCESS != uiStat)
	{
		delete[] pTargetDir;
		return ERROR_INSTALL_FAILURE;
	}

	TCHAR* pIniFileName = new TCHAR[512];
	memset(pIniFileName, 0, sizeof(TCHAR) * 512);
	swprintf_s(pIniFileName, 512, TEXT("%ls%ls"), pTargetDir, TEXT("install.ini"));
	/////////////////////////////////////////////Chrome install json//////////////////////////////////////////
	char* charTargetDir = wcharToChar(pTargetDir);

	TCHAR* wExtensionKey = new TCHAR[512];
	memset(wExtensionKey, 0, sizeof(TCHAR) * 512);
	GetPrivateProfileString(TEXT("ExtensionsConfig"), TEXT("ExtensionKey"), TEXT(""), wExtensionKey, 512, pIniFileName);

	char* cExtensionKey = wcharToChar(wExtensionKey);
	char* cJsonFile = new char[512];
	memset(cJsonFile, 0, sizeof(char) * 512);
	sprintf_s(cJsonFile, 512, "%sscanboxforchrome_win.json", charTargetDir);
	char* chromeFormatDir = strrpcc(charTargetDir, "\\", "/");
	writeLogingA(cJsonFile, g_ChromeHostJson, chromeFormatDir, cExtensionKey);
	delete[] charTargetDir;
	delete[] wExtensionKey;
	delete[] cExtensionKey;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//MessageBoxA(NULL, "123", "show", MB_OK);
	//writeLoging(TEXT("C:\\Windows\\Temp\\ScanBox_install_long.txt"), TEXT("%ls\r\n"), pIniFileName);

	TCHAR* pActiveXRegisterText = new TCHAR[512];
	memset(pActiveXRegisterText, 0, sizeof(TCHAR) * 512);
	DWORD result = GetPrivateProfileString(TEXT("install"), TEXT("ActiveX"), TEXT(""), pActiveXRegisterText, 512, pIniFileName);

	TCHAR* pServiceRegisterText = new TCHAR[512];
	memset(pServiceRegisterText, 0, sizeof(TCHAR) * 512);
	result = GetPrivateProfileString(TEXT("install"), TEXT("ServiceInstall"), TEXT(""), pServiceRegisterText, 512, pIniFileName);

	TCHAR* pServiceAutostartText = new TCHAR[512];
	memset(pServiceAutostartText, 0, sizeof(TCHAR) * 512);
	result = GetPrivateProfileString(TEXT("install"), TEXT("ServiceAutostart"), TEXT(""), pServiceAutostartText, 512, pIniFileName);

	TCHAR* pServiceStartText = new TCHAR[512];
	memset(pServiceStartText, 0, sizeof(TCHAR) * 512);
	result = GetPrivateProfileString(TEXT("install"), TEXT("ServiceStart"), TEXT(""), pServiceStartText, 512, pIniFileName);

	/*TCHAR* pExtensionHostText = new TCHAR[512];
	memset(pExtensionHostText, 0, sizeof(TCHAR) * 512);
	result = GetPrivateProfileString(TEXT("install"), TEXT("ExtensionHost"), TEXT(""), pExtensionHostText, 512, pIniFileName);*/

	pActiveXRegisterText = strrpc(pActiveXRegisterText, TEXT("[installDir]"), pTargetDir);
	pServiceRegisterText = strrpc(pServiceRegisterText, TEXT("[installDir]"), pTargetDir);
	pServiceAutostartText = strrpc(pServiceAutostartText, TEXT("[installDir]"), pTargetDir);
	pServiceStartText = strrpc(pServiceStartText, TEXT("[installDir]"), pTargetDir);
	//pExtensionHostText = strrpc(pExtensionHostText, TEXT("[installDir]"), pTargetDir);

	char* pActReg = wcharToChar(pActiveXRegisterText);
	char* pSvcReg = wcharToChar(pServiceRegisterText);
	char* pAutostart = wcharToChar(pServiceAutostartText);
	char* pStartSvc = wcharToChar(pServiceStartText);
	//char* pExtensionHost = wcharToChar(pExtensionHostText);
	//MessageBoxA(NULL, pActReg, "show", MB_OK);
	//MessageBoxA(NULL, pSvcReg, "show", MB_OK);
	//writeLogingA("C:\\Windows\\Temp\\ScanBox_install_log.txt", "%s\r\n", pActReg);
	//writeLogingA("C:\\Windows\\Temp\\ScanBox_install_log.txt", "%s\r\n", pSvcReg);
	//writeLogingA("C:\\Windows\\Temp\\ScanBox_install_log.txt", "%s\r\n", pExtensionHost);
	system(pActReg);
	system(pSvcReg);
	system(pAutostart);
	system(pStartSvc);
	//system(pExtensionHost);
	RegChromeHost(pTargetDir);

	delete[] pActReg;
	delete[] pSvcReg;
	delete[] pAutostart;
	delete[] pStartSvc;
	delete[] pTargetDir;
	delete[] pActiveXRegisterText;
	delete[] pServiceRegisterText;
	delete[] pServiceAutostartText;
	delete[] pServiceStartText;
	delete[] pIniFileName;
	//delete[] pExtensionHostText;
	//delete[] pExtensionHost;

	return ERROR_SUCCESS;
}

UINT uninstall(MSIHANDLE hInstall)
{
	UINT uiStat = NULL;
	LPWSTR pTargetDir = NULL;
	DWORD dwLen = 0;

	uiStat = getTargetDir(hInstall, &pTargetDir, &dwLen);
	if (ERROR_SUCCESS != uiStat)
	{
		delete[] pTargetDir;
		return ERROR_INSTALL_FAILURE;
	}

	TCHAR* pIniFileName = new TCHAR[512];
	memset(pIniFileName, 0, sizeof(TCHAR) * 512);
	swprintf_s(pIniFileName, 512, TEXT("%ls%ls"), pTargetDir, TEXT("install.ini"));

	//writeLoging(TEXT("%ls\r\n"), pIniFileName);

	TCHAR* pActiveXRegisterText = new TCHAR[512];
	memset(pActiveXRegisterText, 0, sizeof(TCHAR) * 512);
	DWORD result = GetPrivateProfileString(TEXT("uninstall"), TEXT("ActiveX"), TEXT(""), pActiveXRegisterText, 512, pIniFileName);

	TCHAR* pServiceStopText = new TCHAR[512];
	memset(pServiceStopText, 0, sizeof(TCHAR) * 512);
	result = GetPrivateProfileString(TEXT("uninstall"), TEXT("ServiceStop"), TEXT(""), pServiceStopText, 512, pIniFileName);

	TCHAR* pServiceRegisterText = new TCHAR[512];
	memset(pServiceRegisterText, 0, sizeof(TCHAR) * 512);
	result = GetPrivateProfileString(TEXT("uninstall"), TEXT("ServiceUninstall"), TEXT(""), pServiceRegisterText, 512, pIniFileName);

	/*TCHAR* pExtensionHostText = new TCHAR[512];
	memset(pExtensionHostText, 0, sizeof(TCHAR) * 512);
	result = GetPrivateProfileString(TEXT("install"), TEXT("ExtensionHost"), TEXT(""), pExtensionHostText, 512, pIniFileName);*/

	pActiveXRegisterText = strrpc(pActiveXRegisterText, TEXT("[installDir]"), pTargetDir);
	pServiceStopText = strrpc(pServiceStopText, TEXT("[installDir]"), pTargetDir);
	pServiceRegisterText = strrpc(pServiceRegisterText, TEXT("[installDir]"), pTargetDir);
	//pExtensionHostText = strrpc(pExtensionHostText, TEXT("[installDir]"), pTargetDir);

	//writeLoging(TEXT("%d\r\n"), result);
	//writeLoging(TEXT("%ls\r\n"), pActiveXRegisterText);
	//writeLoging(TEXT("%ls\r\n"), pServiceRegisterText);

	char* pActReg = wcharToChar(pActiveXRegisterText);
	char* pStopSvc = wcharToChar(pServiceStopText);
	char* pSvcReg = wcharToChar(pServiceRegisterText);
	//char* pExtensionHost = wcharToChar(pExtensionHostText);
	//MessageBoxA(NULL, pActReg, "show", MB_OK);
	//MessageBoxA(NULL, pSvcReg, "show", MB_OK);

	system(pActReg);
	system(pStopSvc);
	system(pSvcReg);
	//system(pExtensionHost);
	//const LPTSTR regNode = (const LPTSTR)TEXT("Software\\Google\\Chrome\\NativeMessagingHosts\\veiny.scanbox.forchrome");
	//RegDelnode(HKEY_CURRENT_USER, regNode);

	delete[] pActReg;
	delete[] pStopSvc;
	delete[] pSvcReg;
	delete[] pTargetDir;
	delete[] pActiveXRegisterText;
	delete[] pServiceStopText;
	delete[] pServiceRegisterText;
	delete[] pIniFileName;
	//delete[] pExtensionHostText;
	//delete[] pExtensionHost;

	return ERROR_SUCCESS;
}

UINT rollback(MSIHANDLE hInstall)
{
	return uninstall(hInstall);
}