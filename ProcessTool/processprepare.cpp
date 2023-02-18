#pragma once
#include "includes.h"
#include "processlaunch.cpp"
#include "processfunctions.cpp"
#include "strings.h"
#include "tokenadjust.cpp"
#include "tokencreate.cpp"
#include "tokensid.cpp"
#include "variables.h"

namespace
{
	int Launch_Prepare(std::wstring exePath, std::wstring workPath, std::wstring arguments, BOOL asNormal, BOOL asAdmin, BOOL allowUiAccess)
	{
		//Check required process action
		BOOL launchShellExecute = TRUE;
		BOOL launchUnelevated = FALSE;

		//Check for shell command
		if (Check_PathShellCommand(exePath))
		{
			std::wcout << L"Shell launch command detected." << std::endl;
			asAdmin = FALSE;
		}
		else
		{
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
		if (vToolUiAccess && !allowUiAccess)
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
			return Launch_ShellExecute(exePath, workPath, arguments, asAdmin);
		}
		else
		{
			return Launch_CreateProcess(exePath, workPath, arguments, launchTokenHandle);
		}
	}
}