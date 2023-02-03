#pragma once
#include "includes.cpp"
#include "variables.cpp"
#include "processparameter.cpp"
#include "processclose.cpp"
#include "processlaunch.cpp"
#include "processprepare.cpp"
#include "processwindow.cpp"
#include "processstatus.cpp"

namespace
{
	BOOL Restart_ProcessId(DWORD processId, BOOL skipArgs)
	{
		std::wcout << L"Restarting process by id: " << processId << std::endl;

		//Open restart process
		DWORD openDesiredAccess;
		if (vToolAdminAccess)
		{
			openDesiredAccess = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ;
		}
		else
		{
			//Get process access
			__Process_Access processAccess = Process_GetAccessStatus(processId, false);
			if (processAccess.ProcessAdminAccess)
			{
				openDesiredAccess = PROCESS_QUERY_LIMITED_INFORMATION;
				//PROCESS_VM_READ does not work when opening an elevated process as normal user.
				std::wcout << L"Restart limited, missing administrator access." << std::endl;
			}
			else
			{
				openDesiredAccess = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ;
			}
		}

		HANDLE processHandle = OpenProcess(openDesiredAccess, FALSE, processId);
		if (processHandle == INVALID_HANDLE_VALUE || processHandle == NULL)
		{
			std::wcout << L"Failed restarting process by id: " << processId << std::endl;
			return FALSE;
		}

		//Fix check uiaccess
		//Fix check adminaccess

		//Get process details
		std::wstring processApplicationExePath = Process_GetApplicationExePath(processHandle);
		std::wstring processApplicationWorkPath = Process_GetApplicationParameter(processHandle, CurrentDirectoryPath);
		std::wstring processApplicationUserModelId = Process_GetApplicationUserModelId(processHandle);
		std::wstring processApplicationArguments = L"";
		if (!skipArgs)
		{
			processApplicationArguments = Process_GetApplicationParameter(processHandle, CommandLine);
			CommandLine_RemoveExePath(processApplicationArguments);
		}

		std::wcout << L"Restart ExePath: " << processApplicationExePath << std::endl;
		std::wcout << L"Restart ApplicationUserModelId: " << processApplicationUserModelId << std::endl;
		std::wcout << L"Restart WorkPath: " << processApplicationWorkPath << std::endl;
		std::wcout << L"Restart Arguments: " << processApplicationArguments << std::endl;
		CloseHandle(processHandle);

		//Close process
		Close_ProcessId(processId);

		//Check if uwp application
		BOOL uwpProcess = !StringW_IsNullOrWhitespace(processApplicationUserModelId);

		//Launch process
		if (uwpProcess)
		{
			Launch_Uwp(processApplicationUserModelId, processApplicationArguments);
		}
		else
		{
			Launch_Prepare(processApplicationExePath, processApplicationWorkPath, processApplicationArguments, false, false, false);
		}
		return TRUE;
	}
}