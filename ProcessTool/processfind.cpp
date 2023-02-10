#pragma once
#include "includes.h"
#include "strings.h"
#include "defines.h"
#include "headers.h"

namespace
{
	std::vector<__Process_Handle> Find_ProcessesName(std::wstring processName, DWORD desiredAccess)
	{
		std::wcout << L"Finding processes by name: " << processName << std::endl;
		std::vector<__Process_Handle> processList;

		HANDLE processSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (processSnap == INVALID_HANDLE_VALUE || processSnap == NULL)
		{
			std::wcout << L"CreateToolhelp32Snapshot failed: " << GetLastError() << std::endl;
			return processList;
		}

		PROCESSENTRY32W processEntry{};
		processEntry.dwSize = sizeof(processEntry);
		while (Process32NextW(processSnap, &processEntry))
		{
			HANDLE processHandle = OpenProcess(desiredAccess, FALSE, processEntry.th32ProcessID);
			if (processHandle != INVALID_HANDLE_VALUE && processHandle != NULL)
			{
				//Check matching process name
				std::wstring processNameLower = processName;
				std::wstring processExecutableNameLower = processEntry.szExeFile;
				std::wstring processApplicationUserModelIdLower = Process_GetApplicationUserModelId(processHandle);
				StringW_ToLower(processNameLower);
				StringW_ToLower(processExecutableNameLower);
				StringW_ToLower(processApplicationUserModelIdLower);
				if (processExecutableNameLower == processNameLower || processApplicationUserModelIdLower == processNameLower)
				{
					__Process_Handle newProcess{};
					newProcess.Identifier = processEntry.th32ProcessID;
					newProcess.ProcessHandle = processHandle;
					processList.push_back(newProcess);
				}
			}
		}
		CloseHandle(processSnap);
		return processList;
	}
}