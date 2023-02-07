#pragma once
#include "includes.cpp"
#include "string.cpp"
#include "defines.cpp"

namespace
{
	DWORD Window_GetProcessId(HWND hWindow)
	{
		DWORD foundProcessId = 0;
		GetWindowThreadProcessId(hWindow, &foundProcessId);
		if (foundProcessId <= 0)
		{
			//std::wcout << "Process id 0, using GetProcessHandleFromHwnd as backup." << hWindow << std::endl;

			HMODULE hModule = LoadLibraryW(L"oleacc.dll");
			if (hModule == NULL)
			{
				std::wcout << "Failed to get GetModuleHandleW for: " << hWindow << std::endl;
				return foundProcessId;
			}

			__GetProcessHandleFromHwnd getProcessHandleFromHwnd = (__GetProcessHandleFromHwnd)GetProcAddress(hModule, "GetProcessHandleFromHwnd");
			foundProcessId = GetProcessId(getProcessHandleFromHwnd(hWindow));
		}
		return foundProcessId;
	}

	std::wstring Window_GetWindowTitle(HWND hWindow)
	{
		int stringLength = GetWindowTextLengthA(hWindow);
		if (stringLength <= 0)
		{
			return L"";
		}

		stringLength += 1;
		CHAR* getString = new CHAR[stringLength];
		GetWindowTextA(hWindow, getString, stringLength);
		return String_Convert_to_StringW(getString);
	}

	std::wstring Window_GetClassName(HWND hWindow)
	{
		int stringLength = 1024;
		WCHAR* getString = new WCHAR[stringLength]{};
		GetClassNameW(hWindow, getString, stringLength);
		return getString;
	}

	BOOL CALLBACK EnumWindows_Callback(HWND hWindow, __EnumWindows_Params lParam)
	{
		//Get process id
		DWORD foundProcessId = Window_GetProcessId(hWindow);
		if (foundProcessId <= 0)
		{
			return TRUE;
		}

		//Get main window handle
		//BOOL windowType = IsWindow(hWindow);
		BOOL windowVisible = IsWindowVisible(hWindow);
		//int titleLength = GetWindowTextLengthA(hWindow);
		BOOL windowOwner = GetWindow(hWindow, GW_OWNER) == (HWND)0;
		if (lParam.targetProcessId != foundProcessId || !windowOwner || !windowVisible)
		{
			return TRUE;
		}
		else
		{
			lParam.foundWindowHandle = hWindow;
			return FALSE;
		}
	}

	HWND Window_GetMainWindowByProcessId(DWORD processId)
	{
		__EnumWindows_Params lParam{};
		lParam.targetProcessId = processId;
		EnumWindows((WNDENUMPROC)EnumWindows_Callback, (LPARAM)&lParam);
		return lParam.foundWindowHandle;
	}
}