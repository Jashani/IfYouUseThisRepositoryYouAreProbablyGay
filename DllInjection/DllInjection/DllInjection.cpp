// DllInjection.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>

HANDLE FindProcess(WCHAR* wcsProcessName);
BOOL LoadRemoteDLL(HANDLE hProcess, const char* dllPath);

int main(int argc, char *argv[])
{
	// Convert the process name to wchar* to use with pe32
	wchar_t wcsProcessName[MAX_PATH];
	mbstowcs(wcsProcessName, argv[1], MAX_PATH);

	printf(" - Victim process name : %s\n", argv[1]);
	printf(" - DLL to inject       : %s\n", argv[2]);

	// Locates desired process, if successful, runs injection function.
	if (HANDLE hProcess = FindProcess(wcsProcessName)) {
		if(LoadRemoteDLL(hProcess, argv[2])) {
			printf(" - DLL injection successful! \n");
			getchar();
		}
		else {
			printf(" - DLL injection failed. \n");
			getchar();
		}
	}
}

// Searches for process in memory
// Iterates through processes found in memory, returns requested process if found and opened.
HANDLE FindProcess(WCHAR* wcsProcessName)
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		printf(" - Failed to create process snapshot.");

	// Set the size of the structure before using it, otherwise Process32First fails.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		printf(" - Error running Process32First.");
		CloseHandle(hProcessSnap); // clean the snapshot object
		return(FALSE);
	}

	// Walk the snapshot of processes
	do {

		// Could change implementation to inject every single 32bit process it comes across
		// If I'm feeling extra naughty.

		if (!wcscmp(pe32.szExeFile, wcsProcessName)) {
			wprintf(L" - The process %s was found in memory.\n", pe32.szExeFile);


			// Attempt to open and return the process, if found.
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			if (hProcess != NULL) {
				CloseHandle(hProcessSnap);
				return hProcess;
			}
			else {
				wprintf(L" - Failed to open process %s.\n", pe32.szExeFile);
				CloseHandle(hProcessSnap);
				return NULL;
			}
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	wprintf(L" - %s was not found in memory, aborting.\n", wcsProcessName);
	return NULL;
}

// Allocates space for the path in the memory of the target process and writes the path into it.
// Finds LoadLibraryA and runs it as a new thread in the target process.
BOOL LoadRemoteDLL(HANDLE hProcess, const char* sDllPath)
{
	// Allocate space in process memory for DLL path
	LPVOID lpAllocatedMemoryInProcess = VirtualAllocEx(hProcess, NULL, strlen(sDllPath), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!lpAllocatedMemoryInProcess) {
		printf(" - Failed to allocate memory for DLL path.\n");
		CloseHandle(hProcess);
		return FALSE;
	}

	// Write DLL path to process memory
	if (WriteProcessMemory(hProcess, lpAllocatedMemoryInProcess, sDllPath, strlen(sDllPath), NULL)) {
		
		// Return a pointer to the LoadLibrary address. 
		// The address will be the same in the current process and in the target process.
		LPVOID lpLoadLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
		if (!lpLoadLibraryAddress) {
			printf(" - Failed to find LoadLibraryA.\n");
			CloseHandle(hProcess);
			return FALSE;
		}
		else {

			//Create a thread of LoadLibraryA(lpAllocatedMemoryInProcess) to run in the target process memory.
			if (!CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)lpLoadLibraryAddress, lpAllocatedMemoryInProcess, NULL, NULL)) {
				printf(" - Failed to create remote thread in target process.\n");
				CloseHandle(hProcess);
				return FALSE;
			}

		}
	}
	CloseHandle(hProcess);
	return TRUE;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
