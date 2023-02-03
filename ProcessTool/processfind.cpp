#pragma once
#include "includes.cpp"
#include "string.cpp"
#include "defines.cpp"
#include "processinfo.cpp"

namespace
{
	std::vector<__Process_Details> Find_ProcessesName(std::wstring processName, DWORD desiredAccess)
	{
		std::wcout << L"Finding processes by name: " << processName << std::endl;
		std::vector<__Process_Details> processList;

		HANDLE processSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (processSnap == INVALID_HANDLE_VALUE || processSnap == NULL)
		{
			std::wcout << L"CreateToolhelp32Snapshot failed: " << GetLastError() << std::endl;
			return processList;
		}

		PROCESSENTRY32W processEntry{};
		processEntry.dwSize = sizeof(PROCESSENTRY32W);

		while (Process32NextW(processSnap, &processEntry))
		{
			__Process_Details newProcess{};

			HANDLE processHandle = OpenProcess(desiredAccess, FALSE, processEntry.th32ProcessID);
			if (processHandle != INVALID_HANDLE_VALUE && processHandle != NULL)
			{
				//Get application details
				std::wstring processApplicationUserModelId = Process_GetApplicationUserModelId(processHandle);

				//Check matching process name
				std::wstring processExecutableNameLower = processEntry.szExeFile;
				std::wstring processApplicationUserModelIdLower = processApplicationUserModelId;
				StringW_ToLower(processExecutableNameLower);
				StringW_ToLower(processApplicationUserModelIdLower);
				if (processExecutableNameLower == processName || processApplicationUserModelIdLower == processName)
				{
					newProcess.ProcessEntry = processEntry;
					newProcess.ProcessHandle = processHandle;
					newProcess.ProcessId = processEntry.th32ProcessID;
					newProcess.ExecutableName = processEntry.szExeFile;
					newProcess.ApplicationUserModelId = processApplicationUserModelId;
					processList.push_back(newProcess);
				}
			}
		}

		CloseHandle(processSnap);
		return processList;
	}
}