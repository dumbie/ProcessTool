#pragma once
#include "includes.h"
#include "variables.h"
#include "processparameter.cpp"
#include "processclose.cpp"
#include "processlaunch.cpp"
#include "processprepare.cpp"
#include "processwindow.cpp"
#include "processstatus.cpp"

namespace
{
	int Restart_ProcessId(DWORD processId, BOOL skipArgs, std::wstring newArgs)
	{
		std::wcout << L"Restarting process by id: " << processId << std::endl;

		//Open restart process
		DWORD desiredAccess;
		__Process_Access processAccess;
		if (vToolAdminAccess)
		{
			desiredAccess = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ;
		}
		else
		{
			//Get process access
			processAccess = Process_GetAccessStatus(processId, false);
			if (processAccess.ProcessAdminAccess)
			{
				desiredAccess = PROCESS_QUERY_LIMITED_INFORMATION;
			}
			else
			{
				desiredAccess = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ;
			}
		}

		//Get process details
		__Process_Details processDetails = Process_GetDetails(processId, desiredAccess, TRUE);

		//Check launch argument
		std::wstring launchArgument = newArgs;
		if (StringW_IsNullOrWhitespace(launchArgument))
		{
			launchArgument = processDetails.Argument;
		}

		//Close current process
		Close_ProcessTreeId(processId);

		//Launch process
		if (processDetails.ProcessType == UWP || processDetails.ProcessType == Win32Store)
		{
			return Launch_Uwp(processDetails.ApplicationUserModelId, launchArgument);
		}
		else
		{
			return Launch_Prepare(processDetails.ExecutablePath, processDetails.WorkPath, launchArgument, false, processAccess.ProcessAdminAccess, processAccess.ProcessUiAccess);
		}
	}
}