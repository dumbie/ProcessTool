#pragma once
#include "includes.cpp"
#include "string.cpp"
#include "defines.cpp"

namespace
{
	BOOL CALLBACK EnumWindows_Callback(HWND hWnd, __EnumWindows_Params lParam)
	{
		DWORD foundProcessId = 0;
		GetWindowThreadProcessId(hWnd, &foundProcessId);

		//BOOL windowType = IsWindow(hWnd);
		//BOOL windowVisible = IsWindowVisible(hWnd);
		int titleLength = GetWindowTextLengthA(hWnd);
		BOOL windowOwner = GetWindow(hWnd, GW_OWNER) == (HWND)0;
		if (lParam.targetProcessId != foundProcessId || !windowOwner || titleLength == 0)
		{
			return TRUE;
		}
		else
		{
			lParam.foundWindowHandle = hWnd;
			return FALSE;
		}
	}

	HWND Window_GetHwndByProcessId(DWORD processId)
	{
		__EnumWindows_Params lParam{};
		lParam.targetProcessId = processId;
		EnumWindows((WNDENUMPROC)EnumWindows_Callback, (LPARAM)&lParam);
		return lParam.foundWindowHandle;
	}

	std::wstring Process_GetApplicationWindowTitle(DWORD processId)
	{
		HWND handleWindow = Window_GetHwndByProcessId(processId);
		if (handleWindow == (HWND)0)
		{
			return L"";
		}

		int stringLength = GetWindowTextLengthA(handleWindow);
		if (stringLength <= 0)
		{
			return L"";
		}

		stringLength += 1;
		CHAR* getString = new CHAR[stringLength];
		GetWindowTextA(handleWindow, getString, stringLength);
		return String_Convert_to_StringW(getString);
	}
}