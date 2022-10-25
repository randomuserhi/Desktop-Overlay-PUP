#pragma once

#include <Windows.h>

extern int mouseX;
extern int mouseY;

DWORD WINAPI hook(LPVOID lpParm);
DWORD WINAPI killer(LPVOID lpParm);