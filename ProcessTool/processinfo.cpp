#pragma once
#include "includes.cpp"
#include "defines.cpp"
#include "string.cpp"
#include "processwindow.cpp"
#include "processparameter.cpp"

namespace
{
	std::wstring Process_GetApplicationUserModelId(HANDLE hProcess)
	{
		UINT32 stringLength = 0;
		LONG result = GetApplicationUserModelId(hProcess, &stringLength, NULL);
		if (result != ERROR_INSUFFICIENT_BUFFER || result == APPMODEL_ERROR_NO_APPLICATION)
		{
			//std::wcout << "Failed to get ApplicationUserModelId for: " << hProcess << std::endl;
			return L"";
		}

		if (stringLength <= 0)
		{
			//std::wcout << "Failed to get ApplicationUserModelId for: " << hProcess << std::endl;
			return L"";
		}

		WCHAR* getString = new WCHAR[stringLength];
		result = GetApplicationUserModelId(hProcess, &stringLength, getString);
		if (result != ERROR_SUCCESS)
		{
			std::wcout << "Failed to get ApplicationUserModelId for: " << hProcess << std::endl;
			return L"";
		}

		return getString;
	}

	std::wstring Process_GetApplicationExePath(HANDLE hProcess)
	{
		DWORD getSize = MAX_PATH;
		WCHAR* getString = new WCHAR[getSize];
		if (!QueryFullProcessImageNameW(hProcess, 0, getString, &getSize))
		{
			std::wcout << "Failed to get ApplicationExePath for: " << hProcess << std::endl;
			return L"";
		}
		else
		{
			return getString;
		}
	}

	std::wstring Process_GetApplicationExeName(HANDLE hProcess)
	{
		DWORD getSize = MAX_PATH;
		WCHAR* getString = new WCHAR[getSize];
		//K32GetModuleBaseNameW returns name but requires PROCESS_VM_READ
		if (!QueryFullProcessImageNameW(hProcess, 0, getString, &getSize))
		{
			std::wcout << "Failed to get ApplicationExeName for: " << hProcess << "/" << GetLastError() << std::endl;
			return L"";
		}
		else
		{
			std::filesystem::path filePath(getString);
			return filePath.filename();
		}
	}

	DWORD64 Process_GetApplicationRunTimeSec(HANDLE hProcess)
	{
		//Get create time
		FILETIME createTime;
		FILETIME exitTime;
		FILETIME kernelTime;
		FILETIME userTime;
		GetProcessTimes(hProcess, &createTime, &exitTime, &kernelTime, &userTime);

		//Get current time
		FILETIME currentTime;
		GetSystemTimeAsFileTime(&currentTime);

		//Calculate running time
		LARGE_INTEGER largeInteger;
		largeInteger.LowPart = currentTime.dwLowDateTime - createTime.dwLowDateTime;
		largeInteger.HighPart = currentTime.dwHighDateTime - createTime.dwHighDateTime;

		DWORD64 returnSeconds = largeInteger.QuadPart;
		returnSeconds -= 11644473600LL;
		returnSeconds /= 10000000;
		return returnSeconds;
	}

	__Process_Details Process_GetDetails(DWORD processId, DWORD desiredAccess, BOOL closeHandle)
	{
		__Process_Details newProcess{};
		HANDLE processHandle = OpenProcess(desiredAccess, FALSE, processId);
		if (processHandle == INVALID_HANDLE_VALUE || processHandle == NULL)
		{
			std::wcout << "Failed to get process details for: " << processId << std::endl;
			return newProcess;
		}

		//Get application details
		HWND processMainWindowHandle = Window_GetMainWindowByProcessId(processId);
		std::wstring processMainWindowClass = Window_GetClassName(processMainWindowHandle);
		std::wstring processExecutableName = Process_GetApplicationExeName(processHandle);
		std::wstring processExecutablePath = Process_GetApplicationExePath(processHandle);
		std::wstring processApplicationUserModelId = Process_GetApplicationUserModelId(processHandle);

		//Check open process access
		std::wstring processWorkPath = L"";
		std::wstring processArgument = L"";
		if (desiredAccess & PROCESS_VM_READ)
		{
			processWorkPath = Process_GetApplicationParameter(processHandle, CurrentDirectoryPath);
			processArgument = Process_GetApplicationParameter(processHandle, CommandLine);
		}
		else
		{
			//PROCESS_VM_READ does not work when opening an elevated process as normal user.
			std::wcout << L"Process details limited, missing virtual memory access." << std::endl;
		}

		//Check if process is UWP or Win32Store
		if (!StringW_IsNullOrWhitespace(processApplicationUserModelId))
		{
			if (processMainWindowClass == L"ApplicationFrameWindow" || processMainWindowClass == L"Windows.UI.Core.CoreWindow")
			{
				newProcess.ProcessType = UWP;
			}
			else
			{
				newProcess.ProcessType = Win32Store;
			}
		}
		else
		{
			newProcess.ProcessType = Win32;
		}

		//Set application details
		newProcess.Identifier = processId;
		newProcess.ProcessHandle = processHandle;
		newProcess.MainWindowHandle = processMainWindowHandle;
		newProcess.MainWindowClassName = processMainWindowClass;
		newProcess.ExecutableName = processExecutableName;
		newProcess.ExecutablePath = processExecutablePath;
		newProcess.ApplicationUserModelId = processApplicationUserModelId;
		newProcess.WorkPath = processWorkPath;
		newProcess.Argument = processArgument;

		//Show process details
		std::wcout << L"Identifier: " << newProcess.Identifier << std::endl;
		std::wcout << L"ProcessHandle: " << newProcess.ProcessHandle << std::endl;
		std::wcout << L"MainWindowHandle: " << newProcess.MainWindowHandle << std::endl;
		std::wcout << L"MainWindowClassName: " << newProcess.MainWindowClassName << std::endl;
		std::wcout << L"ExecutableName: " << newProcess.ExecutableName << std::endl;
		std::wcout << L"ExecutablePath: " << newProcess.ExecutablePath << std::endl;
		std::wcout << L"ApplicationUserModelId: " << newProcess.ApplicationUserModelId << std::endl;
		std::wcout << L"WorkPath: " << newProcess.WorkPath << std::endl;
		std::wcout << L"Argument: " << newProcess.Argument << std::endl;
		std::wcout << L"ProcessType: " << newProcess.ProcessType << std::endl;

		//Close process handle
		if (closeHandle)
		{
			CloseHandle(processHandle);
		}

		return newProcess;
	}
}