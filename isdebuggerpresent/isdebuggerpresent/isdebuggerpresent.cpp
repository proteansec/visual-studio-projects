// isdebuggerpresent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <Winternl.h>


int _tmain(int argc, _TCHAR* argv[]) {
	__asm { int 3 }
	typedef long NTSTATUS; 
    #define STATUS_SUCCESS    ((NTSTATUS)0L) 
    HANDLE hProcess = GetCurrentProcess();

    typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION { 
                 BOOLEAN DebuggerEnabled; 
                 BOOLEAN DebuggerNotPresent; 
    } SYSTEM_KERNEL_DEBUGGER_INFORMATION, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION; 

    enum SYSTEM_INFORMATION_CLASS { SystemKernelDebuggerInformation = 35 }; 
    typedef NTSTATUS  (__stdcall *ZW_QUERY_SYSTEM_INFORMATION)(IN SYSTEM_INFORMATION_CLASS SystemInformationClass, IN OUT PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength); 
    ZW_QUERY_SYSTEM_INFORMATION ZwQuerySystemInformation;
	SYSTEM_KERNEL_DEBUGGER_INFORMATION Info;

	/* load the ntdll.dll */
	HMODULE hModule = LoadLibrary(_T("ntdll.dll"));
	ZwQuerySystemInformation = (ZW_QUERY_SYSTEM_INFORMATION)GetProcAddress(hModule, "ZwQuerySystemInformation");
	if(ZwQuerySystemInformation == NULL) {
		printf("Error: could not find the function ZwQuerySystemInformation in library ntdll.dll.");
		exit(-1);
	}
	printf("ZwQuerySystemInformation is located at 0x%08x in ntdll.dll.\n", (unsigned int)ZwQuerySystemInformation);

	if (STATUS_SUCCESS == ZwQuerySystemInformation(SystemKernelDebuggerInformation, &Info, sizeof(Info), NULL)) {
            if (Info.DebuggerEnabled && !Info.DebuggerNotPresent) {
                printf("System debugger is present.");
            }
			else {
				printf("System debugger is not present.");
			}
    }

	/* wait */
	getchar();

	return 0;
}

