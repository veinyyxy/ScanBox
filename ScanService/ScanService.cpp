// ScanService.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "ScanService.h"
#include <Psapi.h>
#include <WtsApi32.h>
#include <QtCore/QtCore>
#include <QByteArray>
#include <QFile>
#include <Userenv.h>
#include <stdio.h>
#include "LaunchClientProcess.h"
#pragma comment(lib, "advapi32.lib")

#define SVCNAME TEXT("ScanService")

SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;

SC_HANDLE schSCManager;
SC_HANDLE schService;

bool gWriteLoging = false;
HANDLE g_hToken = NULL;
//
// Purpose: 
//   Entry point for the process
//
// Parameters:
//   None
// 
// Return value:
//   None
//
int __cdecl _tmain(int argc, TCHAR *argv[])
{
	//LaunchMainServerProcessAsUser();
	//return 0;
	// If command-line parameter is "install", install the service. 
	// Otherwise, the service is probably being started by the SCM.
	
	if (lstrcmpi(argv[1], TEXT("install")) == 0)
	{
		SvcInstall();
		return 0;
	}
	else if (lstrcmpi(argv[1], TEXT("uninstall")) == 0)
	{
		DoDeleteSvc();
		return 0;
	}
	else if (lstrcmpi(argv[1], TEXT("start")) == 0)
	{
		DoStartSvc();
		return 0;
	}
	else if (lstrcmpi(argv[1], TEXT("dacl")) == 0)
	{ 
		DoUpdateSvcDacl();
		return 0;
	}
	else if (lstrcmpi(argv[1], TEXT("stop")) == 0)
	{
		DoStopSvc();
		return 0;
	}
	else if (lstrcmpi(argv[1], TEXT("autostart")) == 0)
	{
		DoAutoStartSvc();
		return 0;
	}
	else if (lstrcmpi(argv[1], TEXT("demandstart")) == 0)
	{
		DoDemandStartSvc();
		return 0;
	}
	else
	{
		//_tprintf(TEXT("Unknown command (%s)\n\n"), argv[1]);
		DisplayUsage();
	}
	// TO_DO: Add any additional services for the process to this table.
	//(LPWSTR)SVCNAME
	//LPCWSTR name = L"ScanService";
	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		{ (LPWSTR)SVCNAME, SvcMain },
		{ NULL, NULL }
	};

	// This call returns when the service has stopped. 
	// The process should simply terminate when the call returns.

	if (!StartServiceCtrlDispatcher(DispatchTable))
	{
		SvcReportEvent((LPTSTR)TEXT("StartServiceCtrlDispatcher"));
	}

	return 0;
}

VOID __stdcall DisplayUsage()
{
	printf("Description:\n");
	printf("\tCommand-line tool that controls ScanService.\n\n");
	printf("\tUsage:\n");
	printf("\tScanService [command]\n\n");
	printf("\t[command]\n");
	printf("\t  start\n");
	printf("\t  dacl\n");
	printf("\t  stop\n");
	printf("\t  install\n");
	printf("\t  uninstall\n");
}

//
// Purpose: 
//   Starts the service if possible.
//
// Parameters:
//   None
// 
// Return value:
//   None
//
VOID __stdcall DoStartSvc()
{
	SERVICE_STATUS_PROCESS ssStatus;
	DWORD dwOldCheckPoint;
	DWORD dwStartTickCount;
	DWORD dwWaitTime;
	DWORD dwBytesNeeded;

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // servicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the service.

	schService = OpenService(
		schSCManager,         // SCM database 
		SVCNAME,            // name of service 
		SERVICE_ALL_ACCESS);  // full access 

	if (schService == NULL)
	{
		printf("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	// Check the status in case the service is not stopped. 

	if (!QueryServiceStatusEx(
		schService,                     // handle to service 
		SC_STATUS_PROCESS_INFO,         // information level
		(LPBYTE)&ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded))              // size needed if buffer is too small
	{
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return;
	}

	// Check if the service is already running. It would be possible 
	// to stop the service here, but for simplicity this example just returns. 

	if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
	{
		printf("Cannot start the service because it is already running\n");
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return;
	}

	// Save the tick count and initial checkpoint.

	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	// Wait for the service to stop before attempting to start it.

	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth of the wait hint but not less than 1 second  
		// and not more than 10 seconds. 

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		// Check the status until the service is no longer stop pending. 

		if (!QueryServiceStatusEx(
			schService,                     // handle to service 
			SC_STATUS_PROCESS_INFO,         // information level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded))              // size needed if buffer is too small
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);
			return;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			// Continue to wait and check.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
			{
				printf("Timeout waiting for service to stop\n");
				CloseServiceHandle(schService);
				CloseServiceHandle(schSCManager);
				return;
			}
		}
	}

	// Attempt to start the service.

	if (!StartService(
		schService,  // handle to service 
		0,           // number of arguments 
		NULL))      // no arguments 
	{
		printf("StartService failed (%d)\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return;
	}
	else printf("Service start pending...\n");

	// Check the status until the service is no longer start pending. 

	if (!QueryServiceStatusEx(
		schService,                     // handle to service 
		SC_STATUS_PROCESS_INFO,         // info level
		(LPBYTE)&ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded))              // if buffer too small
	{
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return;
	}

	// Save the tick count and initial checkpoint.

	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
	{
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth the wait hint, but no less than 1 second and no 
		// more than 10 seconds. 

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		// Check the status again. 

		if (!QueryServiceStatusEx(
			schService,             // handle to service 
			SC_STATUS_PROCESS_INFO, // info level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded))              // if buffer too small
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			break;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			// Continue to wait and check.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
			{
				// No progress made within the wait hint.
				break;
			}
		}
	}

	// Determine whether the service is running.

	if (ssStatus.dwCurrentState == SERVICE_RUNNING)
	{
		printf("Service started successfully.\n");
	}
	else
	{
		printf("Service not started. \n");
		printf("  Current State: %d\n", ssStatus.dwCurrentState);
		printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
		printf("  Check Point: %d\n", ssStatus.dwCheckPoint);
		printf("  Wait Hint: %d\n", ssStatus.dwWaitHint);
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

//
// Purpose: 
//   Updates the service DACL to grant start, stop, delete, and read
//   control access to the Guest account.
//
// Parameters:
//   None
// 
// Return value:
//   None
//
VOID __stdcall DoUpdateSvcDacl()
{
	EXPLICIT_ACCESS      ea;
	SECURITY_DESCRIPTOR  sd;
	PSECURITY_DESCRIPTOR psd = NULL;
	PACL                 pacl = NULL;
	PACL                 pNewAcl = NULL;
	BOOL                 bDaclPresent = FALSE;
	BOOL                 bDaclDefaulted = FALSE;
	DWORD                dwError = 0;
	DWORD                dwSize = 0;
	DWORD                dwBytesNeeded = 0;

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the service

	schService = OpenService(
		schSCManager,              // SCManager database 
		SVCNAME,                 // name of service 
		READ_CONTROL | WRITE_DAC); // access

	if (schService == NULL)
	{
		printf("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	// Get the current security descriptor.

	if (!QueryServiceObjectSecurity(schService,
		DACL_SECURITY_INFORMATION,
		&psd,           // using NULL does not work on all versions
		0,
		&dwBytesNeeded))
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			dwSize = dwBytesNeeded;
			psd = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(),
				HEAP_ZERO_MEMORY, dwSize);
			if (psd == NULL)
			{
				// Note: HeapAlloc does not support GetLastError.
				printf("HeapAlloc failed\n");
				goto dacl_cleanup;
			}

			if (!QueryServiceObjectSecurity(schService,
				DACL_SECURITY_INFORMATION, psd, dwSize, &dwBytesNeeded))
			{
				printf("QueryServiceObjectSecurity failed (%d)\n", GetLastError());
				goto dacl_cleanup;
			}
		}
		else
		{
			printf("QueryServiceObjectSecurity failed (%d)\n", GetLastError());
			goto dacl_cleanup;
		}
	}

	// Get the DACL.

	if (!GetSecurityDescriptorDacl(psd, &bDaclPresent, &pacl,
		&bDaclDefaulted))
	{
		printf("GetSecurityDescriptorDacl failed(%d)\n", GetLastError());
		goto dacl_cleanup;
	}

	// Build the ACE.

	BuildExplicitAccessWithName(&ea, (LPWSTR)TEXT("GUEST"),
		SERVICE_START | SERVICE_STOP | READ_CONTROL | DELETE,
		SET_ACCESS, NO_INHERITANCE);

	dwError = SetEntriesInAcl(1, &ea, pacl, &pNewAcl);
	if (dwError != ERROR_SUCCESS)
	{
		printf("SetEntriesInAcl failed(%d)\n", dwError);
		goto dacl_cleanup;
	}

	// Initialize a new security descriptor.

	if (!InitializeSecurityDescriptor(&sd,
		SECURITY_DESCRIPTOR_REVISION))
	{
		printf("InitializeSecurityDescriptor failed(%d)\n", GetLastError());
		goto dacl_cleanup;
	}

	// Set the new DACL in the security descriptor.

	if (!SetSecurityDescriptorDacl(&sd, TRUE, pNewAcl, FALSE))
	{
		printf("SetSecurityDescriptorDacl failed(%d)\n", GetLastError());
		goto dacl_cleanup;
	}

	// Set the new DACL for the service object.

	if (!SetServiceObjectSecurity(schService,
		DACL_SECURITY_INFORMATION, &sd))
	{
		printf("SetServiceObjectSecurity failed(%d)\n", GetLastError());
		goto dacl_cleanup;
	}
	else printf("Service DACL updated successfully\n");

dacl_cleanup:
	CloseServiceHandle(schSCManager);
	CloseServiceHandle(schService);

	if (NULL != pNewAcl)
		LocalFree((HLOCAL)pNewAcl);
	if (NULL != psd)
		HeapFree(GetProcessHeap(), 0, (LPVOID)psd);
}

VOID DoAutoStartSvc()
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the service.

	schService = OpenService(
		schSCManager,            // SCM database 
		SVCNAME,               // name of service 
		SERVICE_CHANGE_CONFIG);  // need change config access 

	if (schService == NULL)
	{
		printf("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	// Change the service start type.

	if (!ChangeServiceConfig(
		schService,        // handle of service 
		SERVICE_NO_CHANGE, // service type: no change 
		SERVICE_AUTO_START,  // service start type 
		SERVICE_NO_CHANGE, // error control: no change 
		NULL,              // binary path: no change 
		NULL,              // load order group: no change 
		NULL,              // tag ID: no change 
		NULL,              // dependencies: no change 
		NULL,              // account name: no change 
		NULL,              // password: no change 
		NULL))            // display name: no change
	{
		printf("ChangeServiceConfig failed (%d)\n", GetLastError());
	}
	else printf("Service autostart successfully.\n");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

VOID DoDemandStartSvc()
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the service.

	schService = OpenService(
		schSCManager,            // SCM database 
		SVCNAME,               // name of service 
		SERVICE_CHANGE_CONFIG);  // need change config access 

	if (schService == NULL)
	{
		printf("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	// Change the service start type.

	if (!ChangeServiceConfig(
		schService,        // handle of service 
		SERVICE_NO_CHANGE, // service type: no change 
		SERVICE_DEMAND_START,  // service start type 
		SERVICE_NO_CHANGE, // error control: no change 
		NULL,              // binary path: no change 
		NULL,              // load order group: no change 
		NULL,              // tag ID: no change 
		NULL,              // dependencies: no change 
		NULL,              // account name: no change 
		NULL,              // password: no change 
		NULL))            // display name: no change
	{
		printf("ChangeServiceConfig failed (%d)\n", GetLastError());
	}
	else printf("Service demandstart successfully.\n");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

//
// Purpose: 
//   Stops the service.
//
// Parameters:
//   None
// 
// Return value:
//   None
//
VOID __stdcall DoStopSvc()
{
	SERVICE_STATUS_PROCESS ssp;
	DWORD dwStartTime = GetTickCount();
	DWORD dwBytesNeeded;
	DWORD dwTimeout = 30000; // 30-second time-out
	DWORD dwWaitTime;

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the service.

	schService = OpenService(
		schSCManager,         // SCM database 
		SVCNAME,            // name of service 
		SERVICE_STOP |
		SERVICE_QUERY_STATUS |
		SERVICE_ENUMERATE_DEPENDENTS);

	if (schService == NULL)
	{
		printf("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	// Make sure the service is not already stopped.

	if (!QueryServiceStatusEx(
		schService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded))
	{
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		goto stop_cleanup;
	}

	if (ssp.dwCurrentState == SERVICE_STOPPED)
	{
		printf("Service is already stopped.\n");
		goto stop_cleanup;
	}

	// If a stop is pending, wait for it.

	while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
	{
		printf("Service stop pending...\n");

		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth of the wait hint but not less than 1 second  
		// and not more than 10 seconds. 

		dwWaitTime = ssp.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		if (!QueryServiceStatusEx(
			schService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded))
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			goto stop_cleanup;
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED)
		{
			printf("Service stopped successfully.\n");
			goto stop_cleanup;
		}

		if (GetTickCount() - dwStartTime > dwTimeout)
		{
			printf("Service stop timed out.\n");
			goto stop_cleanup;
		}
	}

	// If the service is running, dependencies must be stopped first.

	StopDependentServices();

	// Send a stop code to the service.

	if (!ControlService(
		schService,
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&ssp))
	{
		printf("ControlService failed (%d)\n", GetLastError());
		goto stop_cleanup;
	}

	// Wait for the service to stop.

	while (ssp.dwCurrentState != SERVICE_STOPPED)
	{
		Sleep(ssp.dwWaitHint);
		if (!QueryServiceStatusEx(
			schService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded))
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			goto stop_cleanup;
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED)
			break;

		if (GetTickCount() - dwStartTime > dwTimeout)
		{
			printf("Wait timed out\n");
			goto stop_cleanup;
		}
	}
	printf("Service stopped successfully\n");

stop_cleanup:
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

BOOL __stdcall StopDependentServices()
{
	DWORD i;
	DWORD dwBytesNeeded;
	DWORD dwCount;

	LPENUM_SERVICE_STATUS   lpDependencies = NULL;
	ENUM_SERVICE_STATUS     ess;
	SC_HANDLE               hDepService;
	SERVICE_STATUS_PROCESS  ssp;

	DWORD dwStartTime = GetTickCount();
	DWORD dwTimeout = 30000; // 30-second time-out

	// Pass a zero-length buffer to get the required buffer size.
	if (EnumDependentServices(schService, SERVICE_ACTIVE,
		lpDependencies, 0, &dwBytesNeeded, &dwCount))
	{
		// If the Enum call succeeds, then there are no dependent
		// services, so do nothing.
		return TRUE;
	}
	else
	{
		if (GetLastError() != ERROR_MORE_DATA)
			return FALSE; // Unexpected error

		// Allocate a buffer for the dependencies.
		lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(
			GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);

		if (!lpDependencies)
			return FALSE;

		__try {
			// Enumerate the dependencies.
			if (!EnumDependentServices(schService, SERVICE_ACTIVE,
				lpDependencies, dwBytesNeeded, &dwBytesNeeded,
				&dwCount))
				return FALSE;

			for (i = 0; i < dwCount; i++)
			{
				ess = *(lpDependencies + i);
				// Open the service.
				hDepService = OpenService(schSCManager,
					ess.lpServiceName,
					SERVICE_STOP | SERVICE_QUERY_STATUS);

				if (!hDepService)
					return FALSE;

				__try {
					// Send a stop code.
					if (!ControlService(hDepService,
						SERVICE_CONTROL_STOP,
						(LPSERVICE_STATUS)&ssp))
						return FALSE;

					// Wait for the service to stop.
					while (ssp.dwCurrentState != SERVICE_STOPPED)
					{
						Sleep(ssp.dwWaitHint);
						if (!QueryServiceStatusEx(
							hDepService,
							SC_STATUS_PROCESS_INFO,
							(LPBYTE)&ssp,
							sizeof(SERVICE_STATUS_PROCESS),
							&dwBytesNeeded))
							return FALSE;

						if (ssp.dwCurrentState == SERVICE_STOPPED)
							break;

						if (GetTickCount() - dwStartTime > dwTimeout)
							return FALSE;
					}
				}
				__finally
				{
					// Always release the service handle.
					CloseServiceHandle(hDepService);
				}
			}
		}
		__finally
		{
			// Always free the enumeration buffer.
			HeapFree(GetProcessHeap(), 0, lpDependencies);
		}
	}
	return TRUE;
}

//
// Purpose: 
//   Installs a service in the SCM database
//
// Parameters:
//   None
// 
// Return value:
//   None
//
VOID SvcInstall()
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	TCHAR szPath[MAX_PATH];

	if (!GetModuleFileName(nullptr, szPath, MAX_PATH))
	{
		printf("Cannot install service (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		DWORD errorCode = GetLastError();
		printf("OpenSCManager failed (%d)\n", errorCode);
		return;
	}

	// Create the service

	schService = CreateService(
		schSCManager,              // SCM database 
		SVCNAME,                   // name of service 
		SVCNAME,                   // service name to display 
		SERVICE_ALL_ACCESS,        // desired access 
		SERVICE_WIN32_OWN_PROCESS, // service type 
		SERVICE_DEMAND_START,      // start type 
		SERVICE_ERROR_NORMAL,      // error control type 
		szPath,                    // path to service's binary 
		NULL,                      // no load ordering group 
		NULL,                      // no tag identifier 
		NULL,                      // no dependencies 
		NULL,                      // LocalSystem account 
		NULL);                     // no password 

	if (schService == NULL)
	{
		printf("CreateService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}
	else printf("Service installed successfully\n");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

VOID DoDeleteSvc()
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	//SERVICE_STATUS ssStatus;

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the service.

	schService = OpenService(
		schSCManager,       // SCM database 
		SVCNAME,          // name of service 
		DELETE);            // need delete access 

	if (schService == NULL)
	{
		printf("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	// Delete the service.

	if (!DeleteService(schService))
	{
		printf("DeleteService failed (%d)\n", GetLastError());
	}
	else printf("Service deleted successfully\n");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

//
// Purpose: 
//   Entry point for the service
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None.
//
VOID WINAPI SvcMain(DWORD dwArgc, LPWSTR *lpszArgv)
{
	// Register the handler function for the service
	
	gSvcStatusHandle = RegisterServiceCtrlHandler(
		SVCNAME,
		SvcCtrlHandler);
	
	if (!gSvcStatusHandle)
	{
		SvcReportEvent((LPTSTR)TEXT("RegisterServiceCtrlHandler"));
		return;
	}
	
	// These SERVICE_STATUS members remain as set here

	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;

	// Report initial status to the SCM

	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	// Perform service-specific initialization and work.

	/*char** newArgv = new char*[dwArgc];

	for (size_t i = 0; i < dwArgc; i++)
	{
		int nLen = (int)wcslen(lpszArgv[i]) + 1;
		char *tempChars = new char[nLen * 2];
		memset(tempChars, 0, nLen * 2);
		WideCharToMultiByte(CP_ACP, 0, lpszArgv[i], nLen, tempChars, 2 * nLen, NULL, NULL);

		newArgv[i] = tempChars;
	}
	delete[] newArgv;*/

	SvcInit(dwArgc, lpszArgv);
}


//
// Purpose: 
//   The service code
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None
//
VOID SvcInit(int iArgc, LPWSTR* cArgv)
{

	ghSvcStopEvent = CreateEvent(
		NULL,    // default security attributes
		TRUE,    // manual reset event
		FALSE,   // not signaled
		NULL);   // no name

	if (ghSvcStopEvent == NULL)
	{
		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}

	// Report running status when initialization is complete.

	ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
	// TO_DO: Perform work until service stops.

	while (1)
	{
		// Check whether to stop the service.

		if (WaitForSingleObject(ghSvcStopEvent, 3000) == WAIT_OBJECT_0)
		{
			TerminateServerProcess();
			ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
			return;
		}
		else
		{
			LaunchMainServerProcess();
			//LaunchMainServerProcessAsUser();
			continue;
		}
		
	}
}

//
// Purpose: 
//   Sets the current service status and reports it to the SCM.
//
// Parameters:
//   dwCurrentState - The current state (see SERVICE_STATUS)
//   dwWin32ExitCode - The system error code
//   dwWaitHint - Estimated time for pending operation, 
//     in milliseconds
// 
// Return value:
//   None
//
VOID ReportSvcStatus(DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure.
	//gSvcStatus.dwServiceType = SERVICE_USER_OWN_PROCESS;
	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	gSvcStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		gSvcStatus.dwControlsAccepted = 0;
	else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED))
		gSvcStatus.dwCheckPoint = 0;
	else gSvcStatus.dwCheckPoint = dwCheckPoint++;

	// Report the status of the service to the SCM.
	SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}


//
// Purpose: 
//   Called by SCM whenever a control code is sent to the service
//   using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
// 
// Return value:
//   None
//
VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
	// Handle the requested control code. 

	switch (dwCtrl)
	{
	case SERVICE_CONTROL_STOP:
		ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

		// Signal the service to stop.

		SetEvent(ghSvcStopEvent);
		ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);

		return;

	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		break;
	}

}

//
// Purpose: 
//   Logs messages to the event log
//
// Parameters:
//   szFunction - name of function that failed
// 
// Return value:
//   None
//
// Remarks:
//   The service must have an entry in the Application event log.
//
VOID SvcReportEvent(LPTSTR szFunction, WORD eventype)
{
	HANDLE hEventSource;
	LPCTSTR lpszStrings[2];
	TCHAR Buffer[80];

	hEventSource = RegisterEventSource(NULL, SVCNAME);

	if (NULL != hEventSource)
	{
		StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());

		lpszStrings[0] = SVCNAME;
		lpszStrings[1] = Buffer;

		ReportEvent(hEventSource,        // event log handle
			eventype, // event type
			0,                   // event category
			0,           // event identifier
			NULL,                // no security identifier
			2,                   // size of lpszStrings array
			0,                   // no binary data
			lpszStrings,         // array of strings
			NULL);               // no binary data

		DeregisterEventSource(hEventSource);
	}
}

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

DWORD GetProcessFullPath(DWORD dwPID, TCHAR pszFullPath[MAX_PATH])
{
	DWORD dwError = NULL;
	TCHAR szImagePath[MAX_PATH];
	HANDLE hProcess;
	if (!pszFullPath)
		return NULL;
	//EnableDebugPrivilege();

	pszFullPath[0] = '\0';
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, dwPID);

	if (!hProcess)
	{
		dwError = GetLastError();
		return dwError;
	}

	if (!GetProcessImageFileName(hProcess, szImagePath, MAX_PATH))
	{
		CloseHandle(hProcess);

		dwError = GetLastError();
		return dwError;
	}

	if (!DosPathToNtPath(szImagePath, pszFullPath))
	{
		CloseHandle(hProcess);
		return NULL;
	}

	CloseHandle(hProcess);

	//wprintf(L"%d,%s \r\n", dwPID, pszFullPath);
	return NULL;
}

int GetScanFacdeName(QString& strScanFacdeName)
{
	TCHAR myPath[MAX_PATH] = { 0 };
	//缓冲区清0
	ZeroMemory(myPath, MAX_PATH);
	GetModuleFileName(NULL, myPath, MAX_PATH);
	//fprintf(pF, "line 1262 - GetModuleFileName = %ws \r\n", myPath);

	const TCHAR* processName = L"ScanFacade.exe";

	QString strPath = QString::fromUtf16((const char16_t*)myPath);
	QFileInfo fi(strPath);

	QString strMyPath = fi.absolutePath();

	QString ScanFacadeName = strMyPath + QDir::separator() + QString::fromUtf16((const char16_t*)processName);
	strScanFacdeName = ScanFacadeName.replace('/', '\\');
	return 1;
}

BOOL TerminateServerProcess()
{
	QString strScanFacdeName;
	GetScanFacdeName(strScanFacdeName);
	DWORD scanFacdeNameID = CheckMainServerProcess(strScanFacdeName);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, scanFacdeNameID);

	BOOL result = TerminateProcess(hProcess, 0);

	return result;
}

DWORD LaunchMainServerProcess()
{
	QString strScanFacdeName;
	GetScanFacdeName(strScanFacdeName);
	bool checkResult = CheckMainServerProcess(strScanFacdeName);
	if (!checkResult)
	{
		//gWriteLoging = true;
		//ElevationPrivilege();
		DWORD dwResult = LaunchMainServerProcessAsUser();

		return dwResult;
	}

	return 0;
}

DWORD CheckMainServerProcess(const QString& strScanFacdeName)
{
	FILE *pLogingFile = NULL;
	if (gWriteLoging)
	{
		fopen_s(&pLogingFile, "D:\\temp\\loging.txt", "w");
		fprintf(pLogingFile, "%ws\r\n", strScanFacdeName.toStdWString().c_str());
	}
	
	//ScanFacadeName = ScanFacadeName.toUtf8();
	/////////////////////////////////////////////////////////////////////////////////////////////
	DWORD dwProcessID[4280] = { 0 };
	DWORD dwCBNeed = 0;
	EnumProcesses(dwProcessID, sizeof(dwProcessID), &dwCBNeed);
	DWORD dwProcessCnt = dwCBNeed / sizeof(DWORD);
	for (DWORD i = 0; i < dwProcessCnt; i++)
	{
		
		//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID[i]);
		TCHAR path[MAX_PATH] = { 0 };
		ZeroMemory(path, MAX_PATH);
		//GetProcessImageFileName(hProcess, path, MAX_PATH);
		DWORD dwError = GetProcessFullPath(dwProcessID[i], path);
		if (dwError != NULL) continue;
		if (gWriteLoging)
		{
			fprintf(pLogingFile, "%d------------%ws\r\n", dwError, path);
		}
		//D:\\Project\\ScanBox\\x64\\Debug\\ScanFacade.exe
		QString exeImageFile = QString::fromUtf16((const char16_t*)path);
		//exeImageFile = exeImageFile.toUtf8();
		//qDebug() << exeImageFile << "\r\n";
		if (exeImageFile == strScanFacdeName)
		{
			if (gWriteLoging)
			{
				fclose(pLogingFile);
				gWriteLoging = false;
			}
			return dwProcessID[i];
		}
	}
	if (gWriteLoging)
	{
		fclose(pLogingFile);
		gWriteLoging = false;
	}
	return 0;
}

DWORD LaunchMainServerProcessAsUser()
{
#if 1
	DWORD cursessionid;
	cursessionid = WTSGetActiveConsoleSessionId();
	if (g_hToken == NULL)
	{
		if (!WTSQueryUserToken(cursessionid, &g_hToken))
		{
			return GetLastError();
		}
	}
	
	/*if (!ImpersonateLoggedOnUser(g_hToken))
	{
		return GetLastError();
	}
	
	DWORD UsernamePathSize = ARRAYSIZE(szUsernamePath);
	//获取到当前用户路径
	if (!GetUserProfileDirectory(hToken, szUsernamePath, &UsernamePathSize))
	{
		log_error("%s GetUserProfileDirectory is failed\n", __FUNCTION__);
	}
	*/
	
	char* textBuffer[512] = { 0 };
	BOOL bResult = FALSE;
	// 
	// start the process
	//
	LPVOID environment = NULL;
	BOOL blockRet = CreateEnvironmentBlock(&environment, g_hToken, TRUE);

	if (!blockRet)
	{
		TCHAR infoText[] = TEXT("无法创建环境变量块，这有可能导致某些功能不正常。");
		SvcReportEvent(infoText, (WORD)2U);
	}
	QString strScanFacdeName;
	GetScanFacdeName(strScanFacdeName);
	
	TCHAR lpApplicationName[512] = { 0 };
	TCHAR lpCommandLine[512] = { 0 };
	TCHAR lpCurrentDirectory[512] = { 0 };
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	//TCHAR desktopName[] = L"nidaye";
	//si.lpDesktop = desktopName;

	ZeroMemory(&pi, sizeof(pi));

	std::wstring ba = strScanFacdeName.toStdWString();
	wcscpy_s(lpApplicationName, ba.c_str());

	QString strCommandLine = QString::fromLocal8Bit("hide start");
	ba = strCommandLine.toStdWString();
	wcscpy_s(lpCommandLine, ba.c_str());

	QFileInfo exeFileInfo(strScanFacdeName);
	ba = exeFileInfo.absolutePath().toStdWString();
	wcscpy_s(lpCurrentDirectory, ba.c_str());

	bResult = CreateProcessAsUser(
		g_hToken,
		lpApplicationName,
		lpCommandLine,
		NULL,
		NULL,
		FALSE,
		CREATE_UNICODE_ENVIRONMENT,
		environment,
		lpCurrentDirectory,
		&si,
		&pi
	);
	//bResult = GetLastError();
	// 
	// close the handles
	// 
	/*
	if (bResult && pi.hProcess != INVALID_HANDLE_VALUE)
	{
		//WaitForSingleObject(pi.hProcess, INFINITE);
		//CloseHandle(pi.hProcess);
		//CloseHandle(pi.hThread);
	}
	else
	{
		return GetLastError();
	}*/
	//RevertToSelf();
	DestroyEnvironmentBlock(environment);
	CloseHandle(g_hToken);
	g_hToken = NULL;
	return GetLastError();
#else
	QString strScanFacdeName;
	GetScanFacdeName(strScanFacdeName);

	//std::wstring wstrCommandLime = strScanFacdeName.toStdWString();
	TCHAR commandLine[] = TEXT("D:\\Project\\ScanBox\\x64\\Debug\\ScanFacade.exe");
	TCHAR user[] = TEXT("fox-server");
	TCHAR password[] = TEXT("123456");
	//TCHAR user[] = TEXT("LocalSystem");
	//TCHAR password[] = TEXT("");
	return StartInteractiveClientProcess(user, NULL, password, commandLine);
#endif
	
}

#if 0
DWORD LaunchMainServerProcess(const QString & strScanFacdeName)
{
	TCHAR lpApplicationName[512] = { 0 };
	TCHAR lpCommandLine[512] = { 0 };
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	std::wstring ba = strScanFacdeName.toStdWString();
	wcscpy_s(lpApplicationName, ba.c_str());

	QString strCommandLine = strScanFacdeName + QString::fromLocal8Bit(" hide start");
	ba = strCommandLine.toStdWString();
	wcscpy_s(lpCommandLine, ba.c_str());

	BOOL bRet = ::CreateProcess(
		lpApplicationName,          // 在此指定可执行文件的文件名
		lpCommandLine,				// 命令行参数
		NULL,						// 默认进程安全性
		NULL,						// 默认线程安全性
		FALSE,						// 指定当前进程内的句柄不可以被子进程继承
		CREATE_NEW_PROCESS_GROUP,	// 为新进程创建一个新的控制台窗口
		NULL,						// 使用本进程的环境变量
		NULL,						// 使用本进程的驱动器和目录
		&si,
		&pi);

	if (false)
	{
		ModifyProcessToken(pi.hProcess);
	}
	DWORD error = ::GetLastError();

	return error;
}

int ModifyProcessToken(HANDLE processHandle)
{

	HANDLE token_handle;
	//打开访问令牌
	if (!OpenProcessToken(processHandle,		//要修改权限的进程句柄
		TOKEN_ALL_ACCESS,						//要对令牌进行何种操作
		&token_handle							//访问令牌
	))
	{
		printf("openProcessToken error");
		return 0;
	}

	LUID luid;
	if (!LookupPrivilegeValue(NULL,                 //查看的系统，本地为NULL
		SE_SECURITY_NAME,							//要查看的特权名称
		&luid										//用来接收标识符
	))
	{
		printf("lookupPrivilegevalue error");
		return 0;
	}

	TOKEN_PRIVILEGES tkp;
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
	//调整访问令牌权限
	if (!AdjustTokenPrivileges(token_handle,    //令牌句柄
		TRUE,									//是否禁用权限
		&tkp,									//新的特权的权限信息
		sizeof(tkp),							//特权信息大小
		NULL,									//用来接收特权信息当前状态的buffer
		NULL									//缓冲区大小
	))
	{
		printf("adjust error");
		return 0;
	}

	printf("sucessful");

	return 1;
}

BOOL EnableDebugPrivilege()
{
	HANDLE hToken;
	BOOL fOk = FALSE;
	fOk = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
	//fprintf(pF, "line 1251 - OpenProcessToken = 0x%d \r\n", GetLastError());
	if (fOk) //Get Token
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))//Get Luid
			printf("Can't lookup privilege value.\n");
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;//这一句很关键，修改其属性为SE_PRIVILEGE_ENABLEDif(!AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL))//Adjust Token
		printf("Can't adjust privilege value.\n");
		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOk;
}

#endif

bool AdjustProcessTokenPrivilege()
{
	DWORD dwError = 0;
	LUID luidTmp;
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		dwError = GetLastError();
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luidTmp))
	{
		dwError = GetLastError();
		CloseHandle(hToken);
		return FALSE;
	}

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luidTmp;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		dwError = GetLastError();
		CloseHandle(hToken);
		return FALSE;
	}
	CloseHandle(hToken);
	return true;
}

DWORD SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnable)
{
	OutputDebugString(lpszPrivilege);
	DWORD bRet = FALSE;
	HANDLE hToken = NULL;
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, ::GetCurrentProcessId());
	if (!::OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		goto __EXIT;
	}
	LUID Luid;
	if (!::LookupPrivilegeValue(NULL, lpszPrivilege, &Luid))
	{
		goto __EXIT;
	}
	TOKEN_PRIVILEGES newPrivilege;
	newPrivilege.PrivilegeCount = 1;
	newPrivilege.Privileges[0].Luid = Luid;
	newPrivilege.Privileges[0].Attributes = //设置特权属性
		bEnable ?
		SE_PRIVILEGE_ENABLED :
		SE_PRIVILEGE_ENABLED_BY_DEFAULT;
	if (!::AdjustTokenPrivileges(hToken, FALSE, &newPrivilege,
		sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		TCHAR s[64] = { 0 };
		wprintf(s, TEXT("AdjustTokenPrivileges error: %u\n"), GetLastError());
		OutputDebugString(s);
		goto __EXIT;
	}
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)//查看是否真的设置成功了
	{
		OutputDebugString(TEXT("The token does not have the specified privilege. \n"));
		goto __EXIT;
	}
	bRet = TRUE;
	OutputDebugString(TEXT("Set OK"));
__EXIT:
	if (hProcess)
	{
		bRet = GetLastError();
		::CloseHandle(hProcess);
	}
	if (hToken)
	{
		bRet = GetLastError();
		::CloseHandle(hToken);
	}
	return bRet;
}

//设置所有的特权，用户权限下大部分都是获取不到的
VOID ElevationPrivilege()
{
	SetPrivilege(SE_CREATE_TOKEN_NAME);
	SetPrivilege(SE_ASSIGNPRIMARYTOKEN_NAME);
	SetPrivilege(SE_LOCK_MEMORY_NAME);
	SetPrivilege(SE_INCREASE_QUOTA_NAME);
	SetPrivilege(SE_UNSOLICITED_INPUT_NAME);
	SetPrivilege(SE_MACHINE_ACCOUNT_NAME);
	SetPrivilege(SE_TCB_NAME);
	SetPrivilege(SE_SECURITY_NAME);
	SetPrivilege(SE_TAKE_OWNERSHIP_NAME);
	SetPrivilege(SE_LOAD_DRIVER_NAME);
	SetPrivilege(SE_SYSTEM_PROFILE_NAME);
	SetPrivilege(SE_SYSTEMTIME_NAME);
	SetPrivilege(SE_PROF_SINGLE_PROCESS_NAME);
	SetPrivilege(SE_INC_BASE_PRIORITY_NAME);
	SetPrivilege(SE_CREATE_PAGEFILE_NAME);
	SetPrivilege(SE_CREATE_PERMANENT_NAME);
	SetPrivilege(SE_BACKUP_NAME);
	SetPrivilege(SE_RESTORE_NAME);
	SetPrivilege(SE_SHUTDOWN_NAME);
	SetPrivilege(SE_DEBUG_NAME);
	SetPrivilege(SE_AUDIT_NAME);
	SetPrivilege(SE_SYSTEM_ENVIRONMENT_NAME);
	SetPrivilege(SE_CHANGE_NOTIFY_NAME);
	SetPrivilege(SE_REMOTE_SHUTDOWN_NAME);
	SetPrivilege(SE_UNDOCK_NAME);
	SetPrivilege(SE_SYNC_AGENT_NAME);
	SetPrivilege(SE_ENABLE_DELEGATION_NAME);
	SetPrivilege(SE_MANAGE_VOLUME_NAME);
	SetPrivilege(SE_IMPERSONATE_NAME);
	SetPrivilege(SE_CREATE_GLOBAL_NAME);
	SetPrivilege(SE_TRUSTED_CREDMAN_ACCESS_NAME);
	SetPrivilege(SE_RELABEL_NAME);
	SetPrivilege(SE_INC_WORKING_SET_NAME);
	SetPrivilege(SE_TIME_ZONE_NAME);
	SetPrivilege(SE_CREATE_SYMBOLIC_LINK_NAME);
}