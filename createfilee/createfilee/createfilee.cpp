// createfilee.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[]) {

	HANDLE hFile = CreateFile(L"C:\\temp.txt", GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE) {
		printf("Unable to open file.");
	}
	else {
		printf("File successfully opened/created.");
	}
	CloseHandle(hFile);
	getchar();
	return 0;
}


