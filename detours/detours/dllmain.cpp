// dllmain.cpp : Defines the entry point for the DLL application.
//#pragma comment(lib, "detoured.lib")
#pragma comment(lib, "detours.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "detoured.lib")

#include "stdafx.h"
#include <cstdio>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "detours.h"
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")


FILE *pSendLogFile;
FILE *pRecvLogFile;

int (WINAPI *pSend)(SOCKET s, const char* buf, int len, int flags) = send;
int WINAPI MySend(SOCKET s, const char* buf, int len, int flags);
int (WINAPI *pRecv)(SOCKET s, char *buf, int len, int flags) = recv;
int WINAPI MyRecv(SOCKET s, char* buf, int len, int flags);


INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
    switch(Reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hDLL);

            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            //DetourAttach(&(PVOID&)pSend, MySend);
            DetourTransactionCommit();

            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            //DetourAttach(&(PVOID&)pRecv, MyRecv);
            DetourTransactionCommit();
            break;

    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}


int WINAPI MySend(SOCKET s, const char* buf, int len, int flags)
{
    printf("send");
    return pSend(s, buf, len, flags);
}

int WINAPI MyRecv(SOCKET s, char* buf, int len, int flags)
{
    printf("recv");
    return pRecv(s, buf, len, flags);
}