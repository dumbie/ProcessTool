#pragma once
#include "includes.h"
#include "strings.h"
#include "variables.h"
#include "processother.cpp"

namespace
{
	int Launch_CreateProcess(std::wstring exePath, std::wstring workPath, std::wstring arguments, HANDLE hToken)
	{
		std::wcout << L"Starting process using CreateProcess." << std::endl;

		STARTUPINFOW si{};
		PROCESS_INFORMATION pi{};
		DWORD creationFlags = CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP;

		std::wstring commandLineString = exePath;
		if (!StringW_IsNullOrWhitespace(arguments))
		{
			commandLineString += L" " + arguments;
		}
		LPWSTR commandLineLaunch = StringW_Convert_to_LPWSTR(commandLineString);

		LPCWSTR currentDirectoryLaunch = NULL;
		if (!StringW_IsNullOrWhitespace(workPath))
		{
			currentDirectoryLaunch = workPath.c_str();
		}
		else
		{
			std::wstring exeWorkPath = Convert_ExePathToWorkPath(exePath);
			currentDirectoryLaunch = exeWorkPath.c_str();
			std::wcout << L"Workpath is empty, using exepath: " << currentDirectoryLaunch << std::endl;
		}

		if (vToolElevation)
		{
			if (!CreateProcessWithTokenW(hToken, 0, NULL, commandLineLaunch, creationFlags, NULL, currentDirectoryLaunch, &si, &pi))
			{
				std::wcout << L"CreateProcessWithTokenW failed: " << GetLastError() << std::endl;
				return 0;
			}
			else
			{
				DWORD processId = GetProcessId(pi.hProcess);
				std::wcout << L"CreateProcessWithTokenW success: " << processId << std::endl;
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				return processId;
			}
		}
		else
		{
			if (!CreateProcessAsUserW(hToken, NULL, commandLineLaunch, NULL, NULL, FALSE, creationFlags, NULL, currentDirectoryLaunch, &si, &pi))
			{
				std::wcout << L"CreateProcessAsUserW failed: " << GetLastError() << std::endl;
				return 0;
			}
			else
			{
				DWORD processId = GetProcessId(pi.hProcess);
				std::wcout << L"CreateProcessAsUserW success: " << processId << std::endl;
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				return processId;
			}
		}
	}

	int Launch_ShellExecute(std::wstring exePath, std::wstring workPath, std::wstring arguments, BOOL asAdmin)
	{
		std::wcout << L"Starting process using ShellExecute." << std::endl;

		//Set execute information
		SHELLEXECUTEINFOW shellExecuteInfo{};
		shellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
		shellExecuteInfo.nShow = SW_SHOW;
		shellExecuteInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
		shellExecuteInfo.lpVerb = asAdmin ? L"runas" : L"open";
		shellExecuteInfo.lpFile = exePath.c_str();

		//Check for shell command
		if (!Check_PathShellCommand(exePath))
		{
			if (!StringW_IsNullOrWhitespace(arguments))
			{
				shellExecuteInfo.lpParameters = arguments.c_str();
			}
			if (!StringW_IsNullOrWhitespace(workPath))
			{
				shellExecuteInfo.lpDirectory = workPath.c_str();
			}
			else
			{
				std::wstring exeWorkPath = Convert_ExePathToWorkPath(exePath);
				shellExecuteInfo.lpDirectory = exeWorkPath.c_str();
				std::wcout << L"Workpath is empty, using exepath: " << shellExecuteInfo.lpDirectory << std::endl;
			}
		}

		//Shell execute
		ShellExecuteExW(&shellExecuteInfo);
		if (shellExecuteInfo.hProcess == NULL)
		{
			std::wcout << L"Launching shell process failed: " << GetLastError() << std::endl;
			return 0;
		}
		else
		{
			DWORD processId = GetProcessId(shellExecuteInfo.hProcess);
			std::wcout << L"Launched shell process success: " << processId << std::endl;
			CloseHandle(shellExecuteInfo.hProcess);
			return processId;
		}
	}

	int Launch_Uwp(std::wstring appUserModelId, std::wstring arguments)
	{
		//Initialize Com
		HRESULT hResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		if (FAILED(hResult))
		{
			std::wcout << "Error UWP CoInitializeEx: " << hResult << std::endl;
			return 0;
		}

		//Get activation manager
		IApplicationActivationManager* activateManager = NULL;
		hResult = CoCreateInstance(CLSID_ApplicationActivationManager, NULL, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&activateManager));
		if (FAILED(hResult) || activateManager == NULL)
		{
			std::wcout << "Error UWP CoCreateInstance: " << hResult << std::endl;
			return 0;
		}

		//Activate application
		DWORD processId = 0;
		hResult = activateManager->ActivateApplication(appUserModelId.c_str(), arguments.c_str(), AO_NOERRORUI, &processId);
		if (FAILED(hResult))
		{
			std::wcout << "Launching UWP application failed: " << hResult << std::endl;
			return 0;
		}
		else
		{
			std::wcout << "Launched UWP application success: " << processId << std::endl;
			return processId;
		}
	}
}