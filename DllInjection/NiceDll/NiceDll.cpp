// NiceDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>
extern "C"
{
	void Hello()
	{
		MessageBox(NULL, TEXT("LMAO YOU JUST GOT PRANKED BBYYYYYYYYYY"),
			TEXT("PRANK"), MB_OK);
	}
}