// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "NiceDll.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		Hello();
		break;
    case DLL_THREAD_ATTACH:
		Hello();
		break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

// Should probably implement a closing function for the DLL so it doesn't just stay open forever lmaooooo