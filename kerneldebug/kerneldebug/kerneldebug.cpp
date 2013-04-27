// kerneldebug.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <Winternl.h>

int _tmain(int argc, _TCHAR* argv[])
{
	typedef NTSTATUS (NTAPI *pfnNtQueryInformationProcess)(
		IN  HANDLE ProcessHandle,
		IN  PROCESSINFOCLASS ProcessInformationClass,
		OUT PVOID ProcessInformation,
		IN  ULONG ProcessInformationLength,
		OUT PULONG ReturnLength    OPTIONAL
	);

	pfnNtQueryInformationProcess gNtQueryInformationProcess;


	HMODULE hNtDll = LoadLibrary(_T("ntdll.dll"));
	if(hNtDll == NULL) exit(-1);

	gNtQueryInformationProcess = (pfnNtQueryInformationProcess)GetProcAddress(hNtDll, "NtQueryInformationProcess");
	if(gNtQueryInformationProcess == NULL) {
		exit(-1);
	}
	else {
		printf("%x\n", gNtQueryInformationProcess);
	}
	//printf("%s\n", gNtQueryInformationProcess->ProcessInformation);

	getchar();

	return 0;
}

