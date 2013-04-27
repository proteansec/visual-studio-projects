// linear_sweep.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	/*__asm {
		A:
		jmp B
		_emit 0x0f

		B:
		mov eax, 0x10
		push eax

		pop eax
		C:
	}*/

	__asm {
		A:
		mov eax, 2
		cmp eax, 3
		je B
		mov eax, C
		jmp eax

		B:
		_emit 0xf
		
		C:
		mov eax, 10
		push eax
		call D
		
		D:
	}

	/* wait */
	getchar();

	return 0;
}

