#include "stdafx.h"
#include <stdio.h>

int _tmain(int argc, _TCHAR* argv[]) {

	int i=0;
	do {
		printf("Number: %d!\n", i);
	} while(++i < 10);


	/* wait */
	getchar();
	return 0;
}

