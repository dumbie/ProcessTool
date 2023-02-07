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
			//Get process details
			__Process_Details processDetails = Process_GetDetails(processEntry.th32ProcessID, desiredAccess, FALSE);

			//Check matching process name
			std::wstring processNameLower = processName;
			std::wstring processExecutableNameLower = processDetails.ExecutableName;
			std::wstring processApplicationUserModelIdLower = processDetails.ApplicationUserModelId;
			StringW_ToLower(processNameLower);
			StringW_ToLower(processExecutableNameLower);
			StringW_ToLower(processApplicationUserModelIdLower);
			if (processExecutableNameLower == processNameLower || processApplicationUserModelIdLower == processNameLower)
			{
				processList.push_back(processDetails);
			}
		}
		CloseHandle(processSnap);
		return processList;
	}
}