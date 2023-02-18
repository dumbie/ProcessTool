#pragma once
#include "includes.h"

namespace
{
	std::vector<DWORD> Thread_GetProcessThreadIds(DWORD processId)
	{
		std::wcout << L"Getting process thread identifiers: " << processId << std::endl;

		std::vector<DWORD> listThreadIds;
		HANDLE threadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (threadSnap == INVALID_HANDLE_VALUE || threadSnap == NULL)
		{
			std::wcout << L"GetProcessThreads failed: " << GetLastError() << std::endl;
			return listThreadIds;
		}

		THREADENTRY32 threadEntry{};
		threadEntry.dwSize = sizeof(threadEntry);
		while (Thread32Next(threadSnap, &threadEntry))
		{
			if (threadEntry.th32OwnerProcessID == processId)
			{
				listThreadIds.push_back(threadEntry.th32ThreadID);
			}
		}

		CloseHandle(threadSnap);
		return listThreadIds;
	}

	std::vector<HWND> Thread_GetWindowHandles(DWORD threadId)
	{
		std::wcout << L"Getting thread window handles: " << threadId << std::endl;

		HWND childWindow{};
		std::vector<HWND> listWindows;
		while ((childWindow = FindWindowExW(NULL, childWindow, NULL, NULL)) != NULL)
		{
			DWORD foundProcessId = 0;
			DWORD foundThreadId = GetWindowThreadProcessId(childWindow, &foundProcessId);
			if (foundThreadId == threadId)
			{
				listWindows.push_back(childWindow);
			}
		}

		return listWindows;
	}
}