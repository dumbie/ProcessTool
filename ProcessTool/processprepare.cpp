#pragma once
#include "includes.cpp"
#include "processlaunch.cpp"
#include "string.cpp"
#include "tokenadjust.cpp"
#include "tokencreate.cpp"
#include "tokensid.cpp"
#include "variables.cpp"

namespace
{
	void Launch_Prepare(std::wstring exePath, std::wstring workPath, std::wstring arguments, BOOL asNormal, BOOL asAdmin, BOOL noUiAccess)
	{
		//Check required process action
		BOOL launchShellExecute = TRUE;
		BOOL launchUnelevated = FALSE;
		if (asAdmin)
		{
			std::wcout << L"Starting process as administrator." << std::endl;
		}
		else if (asNormal)
		{
			std::wcout << L"Starting process as normal user." << std::endl;
			if (vToolAdminAccess)
			{
				launchShellExecute = FALSE;
				launchUnelevated = TRUE;
			}
		}

		//Set launch token
		HANDLE launchTokenHandle = NULL;
		if (launchUnelevated)
		{
			launchTokenHandle = Token_Create_Unelevated();
			//launchTokenHandle = Token_Create_Restricted(launchTokenHandle);
		}

		//Duplicate token handle
		if (!launchShellExecute)
		{
			launchTokenHandle = Token_Create_Current();
			launchTokenHandle = Token_Duplicate(launchTokenHandle);
		}

		//Adjust token ui access
		if (vToolUiAccess && noUiAccess)
		{
			std::wcout << L"Starting process without uiaccess." << std::endl;
			Token_Adjust_DisableUiAccess(launchTokenHandle);
		}

		//Adjust token privilege
		//Token_Adjust_Privilege(launchTokenHandle, SE_TIME_ZONE_NAME, TRUE);

		//Adjust token integrity level
		//Token_Adjust_IntegrityLevel(WinMediumLabelSid, launchTokenHandle);

		//Launch application
		if (launchShellExecute)
		{
			Launch_ShellExecute(exePath, workPath, arguments, asAdmin);
		}
		else
		{
			Launch_CreateProcess(exePath, workPath, arguments, launchTokenHandle);
		}
	}
}