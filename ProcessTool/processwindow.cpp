#pragma once
#include "includes.h"
#include "strings.h"
#include "defines.h"
#include "headers.h"
#include "processthread.cpp"

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
		int stringLength = GetWindowTextLengthW(hWindow);
		if (stringLength <= 0)
		{
			return L"";
		}

		stringLength += 1;
		WCHAR* getString = new WCHAR[stringLength];
		GetWindowTextW(hWindow, getString, stringLength);
		return getString;
	}

	std::wstring Window_GetClassName(HWND hWindow)
	{
		int stringLength = 1024;
		WCHAR* getString = new WCHAR[stringLength]{};
		GetClassNameW(hWindow, getString, stringLength);
		return getString;
	}

	std::wstring Window_GetAppUserModelId(HWND hWindow)
	{
		//Get property store
		IPropertyStore* propertyStore = NULL;
		HRESULT hResult = SHGetPropertyStoreForWindow(hWindow, IID_IPropertyStore, (void**)&propertyStore);
		if (FAILED(hResult) || propertyStore == NULL)
		{
			return L"";
		}

		//Get property value
		PROPVARIANT propertyVariant;
		hResult = propertyStore->GetValue(PKEY_AppUserModel_ID, &propertyVariant);
		if (FAILED(hResult))
		{
			return L"";
		}
		std::wstring propertyString = propertyVariant.pwszVal;

		//Cleanup
		propertyStore->Release();
		PropVariantClear(&propertyVariant);

		//Return
		return propertyString;
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

	HWND Window_GetUwpWindowByAppUserModelId(std::wstring appUserModelId)
	{
		std::wcout << L"Looking for UWP window handle in frame host." << std::endl;

		//Get ApplicationFrameHost process handle
		std::vector<__Process_Handle> foundProcesses = Find_ProcessesName(L"ApplicationFrameHost.exe", PROCESS_QUERY_LIMITED_INFORMATION);
		if (foundProcesses.size() == 0)
		{
			std::wcout << L"No ApplicationFrameHost process found." << appUserModelId << std::endl;
			return (HWND)0;
		}
		__Process_Handle processApplicationFrameHost = foundProcesses.front();

		//Get ApplicationFrameHost process threads
		std::vector<DWORD> processThreadIds = Thread_GetProcessThreadIds(processApplicationFrameHost.Identifier);
		if (foundProcesses.size() == 0)
		{
			std::wcout << L"No process threads found." << appUserModelId << std::endl;
			return (HWND)0;
		}

		//Loop through threads to get window
		for (DWORD& processThreadId : processThreadIds)
		{
			std::wcout << L"Process thread id: " << processThreadId << std::endl;

			std::vector<HWND> threadWindows = Thread_GetWindowHandles(processThreadId);
			if (threadWindows.size() == 0)
			{
				std::wcout << L"Process thread has no windows: " << processThreadId << std::endl;
				continue;
			}

			HWND uwpWindowHandle{};
			BOOL uwpApplicationFrame = FALSE;
			BOOL uwpUserInterface = FALSE;
			for (HWND& threadWindowHandle : threadWindows)
			{
				std::wstring processMainWindowClass = Window_GetClassName(threadWindowHandle);
				if (processMainWindowClass == L"ApplicationFrameWindow")
				{
					uwpWindowHandle = threadWindowHandle;
					uwpApplicationFrame = true;
				}
				else if (processMainWindowClass == L"MSCTFIME UI")
				{
					uwpUserInterface = true;
				}
			}

			if (uwpApplicationFrame && uwpUserInterface)
			{
				std::wstring targetAppUserModelIdLower = appUserModelId;
				std::wstring foundAppUserModelIdLower = Window_GetAppUserModelId(uwpWindowHandle);
				StringW_ToLower(targetAppUserModelIdLower);
				StringW_ToLower(foundAppUserModelIdLower);
				if (targetAppUserModelIdLower == foundAppUserModelIdLower)
				{
					std::wcout << L"Process uwp thread window: " << uwpWindowHandle << "/" << appUserModelId << std::endl;
					return uwpWindowHandle;
				}
			}
		}

		return (HWND)0;
	}
}