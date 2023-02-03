#pragma once
#include "includes.cpp"
#include "string.cpp"
#include "variables.cpp"

namespace
{
	BOOL Launch_CreateProcess(std::wstring exePath, std::wstring workPath, std::wstring arguments, HANDLE hToken)
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

		if (vToolElevation)
		{
			if (!CreateProcessWithTokenW(hToken, 0, NULL, commandLineLaunch, creationFlags, NULL, currentDirectoryLaunch, &si, &pi))
			{
				std::wcout << L"CreateProcessWithTokenW failed: " << GetLastError() << std::endl;
				return FALSE;
			}
			else
			{
				std::wcout << L"CreateProcessWithTokenW success: " << pi.hProcess << std::endl;
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				return TRUE;
			}
		}
		else
		{
			if (!CreateProcessAsUserW(hToken, NULL, commandLineLaunch, NULL, NULL, FALSE, creationFlags, NULL, currentDirectoryLaunch, &si, &pi))
			{
				std::wcout << L"CreateProcessAsUserW failed: " << GetLastError() << std::endl;
				return FALSE;
			}
			else
			{
				std::wcout << L"CreateProcessAsUserW success: " << pi.hProcess << std::endl;
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				return TRUE;
			}
		}
	}

	BOOL Launch_ShellExecute(std::wstring exePath, std::wstring workPath, std::wstring arguments, BOOL asAdmin)
	{
		std::wcout << L"Starting process using ShellExecute." << std::endl;

		//Set execute information
		SHELLEXECUTEINFOW shellExecuteInfo{};
		shellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
		shellExecuteInfo.nShow = SW_SHOW;
		shellExecuteInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
		shellExecuteInfo.lpVerb = asAdmin ? L"runas" : L"open";
		shellExecuteInfo.lpFile = exePath.c_str();
		if (!StringW_IsNullOrWhitespace(arguments))
		{
			shellExecuteInfo.lpParameters = arguments.c_str();
		}
		if (!StringW_IsNullOrWhitespace(workPath))
		{
			shellExecuteInfo.lpDirectory = workPath.c_str();
		}

		//Shell execute
		ShellExecuteExW(&shellExecuteInfo);
		if (shellExecuteInfo.hProcess == NULL)
		{
			std::wcout << L"Launching shell process failed: " << GetLastError() << std::endl;
			return FALSE;
		}
		else
		{
			std::wcout << L"Launched shell process success: " << shellExecuteInfo.hProcess << std::endl;
			return TRUE;
		}
	}

	BOOL Launch_Uwp(std::wstring appUserModelId, std::wstring arguments)
	{
		//Initialize Com
		HRESULT hResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		if (FAILED(hResult))
		{
			std::wcout << "Error UWP CoInitializeEx: " << hResult << std::endl;
			return FALSE;
		}

		//Get activation manager
		IApplicationActivationManager* activateManager = NULL;
		hResult = CoCreateInstance(CLSID_ApplicationActivationManager, NULL, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&activateManager));
		if (FAILED(hResult) || activateManager == NULL)
		{
			std::wcout << "Error UWP CoCreateInstance: " << hResult << std::endl;
			return FALSE;
		}

		//Activate application
		DWORD processId = 0;
		hResult = activateManager->ActivateApplication(appUserModelId.c_str(), arguments.c_str(), AO_NOERRORUI, &processId);
		if (FAILED(hResult))
		{
			std::wcout << "Launching UWP application failed: " << hResult << std::endl;
			return FALSE;
		}
		else
		{
			std::wcout << "Launched UWP application success: " << processId << std::endl;
			return TRUE;
		}
	}
}