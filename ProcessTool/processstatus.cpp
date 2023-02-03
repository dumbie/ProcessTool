#pragma once
#include "includes.cpp"
#include "defines.cpp"
#include "tokencreate.cpp"

namespace
{
	__Process_Access Process_GetAccessStatus(DWORD processId, BOOL currentProcess)
	{
		//Open process token
		HANDLE processTokenHandle;
		if (currentProcess)
		{
			processTokenHandle = Token_Create_Current();
		}
		else
		{
			processTokenHandle = Token_Create_Process(processId, PROCESS_QUERY_LIMITED_INFORMATION, TOKEN_QUERY);
		}

		//Create process access
		__Process_Access processAccess;

		//Check process elevation status
		DWORD tokenLength = 0;
		GetTokenInformation(processTokenHandle, TokenElevation, &processAccess.ProcessElevation, sizeof(processAccess.ProcessElevation), &tokenLength);
		std::wcout << L"Process token elevation status: " << processAccess.ProcessElevation << std::endl;

		//Check process elevation type
		GetTokenInformation(processTokenHandle, TokenElevationType, &processAccess.ProcessElevationType, sizeof(processAccess.ProcessElevationType), &tokenLength);
		std::wcout << L"Process token elevation type: " << processAccess.ProcessElevationType << std::endl;

		//Check process uiaccess status
		GetTokenInformation(processTokenHandle, TokenUIAccess, &processAccess.ProcessUiAccess, sizeof(processAccess.ProcessUiAccess), &tokenLength);
		std::wcout << L"Process token uiaccess status: " << processAccess.ProcessUiAccess << std::endl;

		//Check process admin status
		processAccess.ProcessAdminAccess = processAccess.ProcessElevation || processAccess.ProcessElevationType == TokenElevationTypeFull;

		//Close current process token
		CloseHandle(processTokenHandle);
		return processAccess;
	}

	void Process_ToolUpdateAccessStatus()
	{
		__Process_Access processAccess = Process_GetAccessStatus(0, true);
		vToolElevation = processAccess.ProcessElevation;
		vToolUiAccess = processAccess.ProcessUiAccess;
		vToolAdminAccess = processAccess.ProcessAdminAccess;
	}
}