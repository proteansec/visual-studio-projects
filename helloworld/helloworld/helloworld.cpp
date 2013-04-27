#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	__asm { int 0x3 }
	printf("Hello World!");
	return 0;
}

