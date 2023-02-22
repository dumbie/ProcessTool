#pragma once
#include "includes.h"
#include "defines.h"
#include "strings.h"
#include "processwindow.cpp"
#include "processparameter.cpp"
#include "processfunctions.cpp"
#include "processdetails.cpp"

namespace
{
	__Process_Multi ProcessMulti_GetFromProcessId(DWORD processId, DWORD desiredAccess, BOOL closeHandle)
	{
		__Process_Multi newProcess{};
		HANDLE processHandle = OpenProcess(desiredAccess, FALSE, processId);
		if (processHandle == INVALID_HANDLE_VALUE || processHandle == NULL)
		{
			std::wcout << "Failed to get process details for: " << processId << std::endl;
			return newProcess;
		}

		//Get application details
		HWND processMainWindowHandle = Window_GetMainWindowByProcessId(processId);
		std::wstring processMainWindowClass = Window_GetClassName(processMainWindowHandle);
		std::wstring processMainWindowTitle = Window_GetWindowTitle(processMainWindowHandle);
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
			if (processMainWindowClass == L"" || processMainWindowClass == L"ApplicationFrameWindow" || processMainWindowClass == L"Windows.UI.Core.CoreWindow")
			{
				newProcess.ProcessType = UWP;
				processMainWindowHandle = Window_GetUwpWindowByAppUserModelId(processApplicationUserModelId);
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
		newProcess.MainWindowTitle = processMainWindowTitle;
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
		std::wcout << L"MainWindowTitle: " << newProcess.MainWindowTitle << std::endl;
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