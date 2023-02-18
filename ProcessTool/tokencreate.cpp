#pragma once
#include "includes.h"
#include "variables.h"
#include "processwindow.cpp"

namespace
{
	HANDLE Token_Duplicate(HANDLE& hToken)
	{
		//Duplicate token handle
		HANDLE dHandle = INVALID_HANDLE_VALUE;
		if (!DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenImpersonation, &dHandle))
		{
			std::wcout << L"DuplicateTokenEx failed: " << GetLastError() << std::endl;
			return NULL;
		}
		else
		{
			std::wcout << L"DuplicateTokenEx success: " << hToken << ">" << dHandle << std::endl;
			CloseHandle(hToken);
			return dHandle;
		}
	}

	HANDLE Token_Create_Current()
	{
		//Open current process token
		HANDLE hToken = INVALID_HANDLE_VALUE;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
		{
			std::wcout << L"OpenProcessToken current failed: " << GetLastError() << std::endl;
			return NULL;
		}
		else
		{
			std::wcout << L"OpenProcessToken current success: " << hToken << std::endl;
			return hToken;
		}
	}

	HANDLE Token_Create_Process(DWORD processId, DWORD processAccess, DWORD tokenAccess)
	{
		//Open process
		HANDLE hProcess = OpenProcess(processAccess, FALSE, processId);
		if (hProcess == NULL)
		{
			std::wcout << L"OpenProcess process failed: " << GetLastError() << std::endl;
			return NULL;
		}
		else
		{
			std::wcout << L"OpenProcess process success: " << hProcess << std::endl;
		}

		//Open process token
		HANDLE hToken = INVALID_HANDLE_VALUE;
		if (!OpenProcessToken(hProcess, tokenAccess, &hToken))
		{
			std::wcout << L"OpenProcessToken process failed: " << GetLastError() << std::endl;
			CloseHandle(hProcess);
			return NULL;
		}
		else
		{
			std::wcout << L"OpenProcessToken process success: " << hToken << std::endl;
			CloseHandle(hProcess);
			return hToken;
		}
	}

	HANDLE Token_Create_Unelevated()
	{
		//Get unelevated processid
		DWORD unelevatedProcessId = Window_GetProcessId(GetShellWindow());
		if (unelevatedProcessId <= 0)
		{
			std::wcout << L"GetWindowThreadProcessId unelevated failed: " << GetLastError() << std::endl;
			return NULL;
		}
		else
		{
			std::wcout << L"GetWindowThreadProcessId unelevated success: " << unelevatedProcessId << std::endl;
		}

		//Open unelevated process
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, unelevatedProcessId);
		if (hProcess == NULL)
		{
			std::wcout << L"OpenProcess unelevated failed: " << GetLastError() << std::endl;
			return NULL;
		}
		else
		{
			std::wcout << L"OpenProcess unelevated success: " << hProcess << std::endl;
		}

		//Open unelevated process token
		HANDLE hToken = INVALID_HANDLE_VALUE;
		if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
		{
			std::wcout << L"OpenProcessToken unelevated failed: " << GetLastError() << std::endl;
			CloseHandle(hProcess);
			return NULL;
		}
		else
		{
			std::wcout << L"OpenProcessToken unelevated success: " << hToken << std::endl;
			CloseHandle(hProcess);
			return hToken;
		}
	}

	//Create restricted token
	HANDLE Token_Create_Restricted(HANDLE& hToken)
	{
		//Disables Elevation, ElevationType stays Full, Integrity stays High
		HANDLE rToken = INVALID_HANDLE_VALUE;
		if (!CreateRestrictedToken(hToken, LUA_TOKEN, 0, NULL, 0, NULL, 0, NULL, &rToken))
		{
			std::wcout << L"CreateRestrictedToken failed: " << GetLastError() << std::endl;
			return NULL;
		}
		else
		{
			std::wcout << L"CreateRestrictedToken success: " << rToken << std::endl;
			CloseHandle(hToken);
			return rToken;
		}
	}
}