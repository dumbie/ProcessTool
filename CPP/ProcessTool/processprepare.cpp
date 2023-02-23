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

		//Check for url protocol
		bool shellCommand = Check_PathUrlProtocol(exePath);
		if (shellCommand)
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
		}
		else
		{
			launchTokenHandle = Token_Create_Current();
		}

		//Duplicate launch token
		if (!launchShellExecute)
		{
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

		//Check work path
		if (!shellCommand && StringW_IsNullOrWhitespace(workPath))
		{
			workPath = Convert_ExePathToWorkPath(exePath);
			std::wcout << L"Workpath is empty, using exepath: " << workPath << std::endl;
		}

		//Show debug message
		if (vToolDebugMode)
		{
			std::wstring debugMessage = L"ExePath: " + exePath + L"\n\nWorkPath: " + workPath + L"\n\nArguments: " + arguments;
			MessageBoxW(NULL, debugMessage.c_str(), L"Process Tool Debug", MB_OK);
		}

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