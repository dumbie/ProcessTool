#pragma once
#include "includes.cpp"
#include "processfind.cpp"

namespace
{
	BOOL Close_ProcessId(DWORD processId)
	{
		std::wcout << L"Closing process by id: " << processId << std::endl;

		HANDLE closeProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
		if (closeProcess == INVALID_HANDLE_VALUE || closeProcess == NULL)
		{
			std::wcout << L"Closing process not found: " << processId << std::endl;
			return FALSE;
		}
		else
		{
			BOOL processClosed = TerminateProcess(closeProcess, 1);
			CloseHandle(closeProcess);
			return processClosed;
		}
	}

	BOOL Close_ProcessMessageHwnd(HWND windowHandle)
	{
		std::wcout << L"Closing process by window message: " << windowHandle << std::endl;

		LRESULT resultClose = SendMessageW(windowHandle, WM_CLOSE, 0, 0);
		LRESULT resultQuit = SendMessageW(windowHandle, WM_QUIT, 0, 0);
		return (resultClose == CB_ERR || resultClose == LB_ERRSPACE) && (resultQuit == CB_ERR || resultQuit == LB_ERRSPACE);
	}

	BOOL Close_ProcessesName(std::wstring processName)
	{
		std::wcout << L"Closing processes by name: " << processName << std::endl;

		std::vector<__Process_Details> foundProcesses = Find_ProcessesName(processName, PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE);

		if (foundProcesses.size() == 0)
		{
			std::wcout << L"No process found to close by name: " << processName << std::endl;
			return FALSE;
		}

		BOOL processClosed = FALSE;
		for (__Process_Details& process : foundProcesses)
		{
			if (TerminateProcess(process.ProcessHandle, 1))
			{
				processClosed = TRUE;
				std::wcout << L"Found and closed process: " << process.ProcessId << "/" << process.ExecutableName << "/" << process.ApplicationUserModelId << std::endl;
			}
			CloseHandle(process.ProcessHandle);
		}

		return processClosed;
	}
}