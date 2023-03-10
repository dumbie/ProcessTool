#pragma once
#include "includes.h"
#include "variables.h"
#include "processparameter.cpp"
#include "processclose.cpp"
#include "processlaunch.cpp"
#include "processprepare.cpp"
#include "processwindow.cpp"
#include "processstatus.cpp"
#include "processmulti.cpp"

namespace
{
	int Restart_ProcessId(DWORD processId, std::wstring newArgs, BOOL withoutArgs)
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
			processAccess = Process_GetAccessStatus(processId, FALSE);
			if (processAccess.ProcessAdminAccess)
			{
				desiredAccess = PROCESS_QUERY_LIMITED_INFORMATION;
			}
			else
			{
				desiredAccess = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ;
			}
		}

		//Get multi process
		__Process_Multi processDetails = ProcessMulti_GetFromProcessId(processId, desiredAccess, TRUE);

		//Check launch argument
		std::wstring launchArgument = L"";
		if (!withoutArgs)
		{
			launchArgument = newArgs;
			if (StringW_IsNullOrWhitespace(launchArgument))
			{
				launchArgument = processDetails.Argument;
			}
		}

		//Close current process
		Close_ProcessTreeId(processId);

		//Wait for process to have closed
		Thread_Sleep(500);

		//Launch process
		if (processDetails.ProcessType == UWP || processDetails.ProcessType == Win32Store)
		{
			return Launch_Uwp(processDetails.ApplicationUserModelId, launchArgument);
		}
		else
		{
			return Launch_Prepare(processDetails.ExecutablePath, processDetails.WorkPath, launchArgument, FALSE, processAccess.ProcessAdminAccess, processAccess.ProcessUiAccess);
		}
	}
}