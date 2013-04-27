//////////////////////////////////////////////////////////////////////////////
//
//  Detours Test Program (simple.cpp of simple.dll)
//
//  Microsoft Research Detours Package, Version 2.1.
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  This DLL will detour the Windows Sleep API so that TimedSleep function
//  gets called instead.  TimedSleep records the before and after times, and
//  calls the real Sleep API through the TrueSleep function pointer.
//

#include "stdafx.h"
#include "detours.h"
#include <cstdio>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "detours.lib")
#pragma comment(lib, "detoured.lib")
#pragma comment(lib, "ws2_32.lib")

FILE *pSendLogFile;
FILE *pRecvLogFile;

/* send */
int (WINAPI *dsend)(SOCKET, const char*, int, int) = send;
int WINAPI mysend(SOCKET s, const char* buf, int len, int flags);

/* recv */
int (WINAPI *drecv)(SOCKET, char*, int, int) = recv;
int WINAPI myrecv(SOCKET s, char* buf, int len, int flags);

/* connect */
int (WINAPI *dconnect)(SOCKET, const struct sockaddr*, int) = connect;
int WINAPI myconnect(SOCKET s, const struct sockaddr *name, int namelen);

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
    switch(Reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hDLL);
			
			DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&(PVOID&)dconnect, myconnect);
            DetourTransactionCommit();

            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&(PVOID&)dsend, mysend);
            DetourTransactionCommit();

            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&(PVOID&)drecv, myrecv);
            DetourTransactionCommit();
            break;

    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}


int WINAPI mysend(SOCKET s, const char* buf, int len, int flags) {
    FILE *file;
	fopen_s(&file, "C:\\meterpreter.txt", "a+");
	fprintf(file, "Send: %s\n", buf);
    fclose(file);
    return dsend(s, buf, len, flags);
}

int WINAPI myrecv(SOCKET s, char* buf, int len, int flags) {
    FILE *file;
	fopen_s(&file, "C:\\meterpreter.txt", "a+");
	fprintf(file, "Recv: %s\n", buf);
    fclose(file);
    return drecv(s, buf, len, flags);
}

int WINAPI myconnect(SOCKET s, const struct sockaddr *name, int namelen) {
	FILE *file;
	fopen_s(&file, "C:\\meterpreter.txt", "a+");
	SOCKADDR_IN* name_in = (SOCKADDR_IN*)name;
	fprintf(file, "%s : %d\n", inet_ntoa(name_in->sin_addr), ntohs(name_in->sin_port));
    fclose(file);
    return dconnect(s, name, namelen);
}

extern "C" __declspec(dllexport) void dummy(void){
	return;
}

//
///////////////////////////////////////////////////////////////// End of File.
