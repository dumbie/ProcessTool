#pragma once
#include "includes.h"
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
			std::wcout << L"Closed process by id: " << processId << "/" << processClosed << std::endl;
			return processClosed;
		}
	}

	BOOL Close_ProcessTreeId(DWORD processId)
	{
		std::wcout << L"Closing process tree by id: " << processId << std::endl;

		HANDLE processSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (processSnap == INVALID_HANDLE_VALUE || processSnap == NULL)
		{
			std::wcout << L"Closing ProcessTreeId failed: " << GetLastError() << std::endl;
			return FALSE;
		}

		//Terminate child processes
		PROCESSENTRY32W processEntry{};
		processEntry.dwSize = sizeof(processEntry);
		while (Process32NextW(processSnap, &processEntry))
		{
			if (processEntry.th32ParentProcessID == processId)
			{
				HANDLE childProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processEntry.th32ProcessID);
				if (childProcess)
				{
					TerminateProcess(childProcess, 1);
					CloseHandle(childProcess);
				}
			}
		}
		CloseHandle(processSnap);

		//Terminate main process
		HANDLE mainProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
		if (mainProcess)
		{
			TerminateProcess(mainProcess, 1);
			CloseHandle(mainProcess);
		}

		std::wcout << L"Closed process tree by id: " << processId << std::endl;
		return TRUE;
	}

	BOOL Close_ProcessMessageHwnd(HWND hWindow)
	{
		std::wcout << L"Closing process by window message: " << hWindow << std::endl;

		LRESULT resultClose = SendMessageW(hWindow, WM_CLOSE, 0, 0);
		LRESULT resultQuit = SendMessageW(hWindow, WM_QUIT, 0, 0);

		BOOL processClosed = !((resultClose == CB_ERR || resultClose == LB_ERRSPACE) && (resultQuit == CB_ERR || resultQuit == LB_ERRSPACE));
		std::wcout << L"Closed process by window message: " << hWindow << "/" << processClosed << std::endl;
		return processClosed;
	}

	BOOL Close_ProcessesName(std::wstring processName)
	{
		std::wcout << L"Closing processes by name: " << processName << std::endl;

		std::vector<__Process_Handle> foundProcesses = Find_ProcessesName(processName, PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE);
		if (foundProcesses.size() == 0)
		{
			std::wcout << L"No process found to close by name: " << processName << std::endl;
			return FALSE;
		}

		BOOL processClosed = FALSE;
		for (__Process_Handle& process : foundProcesses)
		{
			if (TerminateProcess(process.ProcessHandle, 1))
			{
				processClosed = TRUE;
				std::wcout << L"Found and closed process by name: " << process.Identifier << "/" << process.ProcessHandle << "/" << processName << std::endl;
			}
			CloseHandle(process.ProcessHandle);
		}

		return processClosed;
	}
}