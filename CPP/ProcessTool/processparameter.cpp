#pragma once
#include "includes.h"
#include "defines.h"
#include "strings.h"
#include "processparameter32.cpp"
#include "processparameter64.cpp"
#include "processparameterwow64.cpp"

namespace
{
	void CommandLine_RemoveExePath(std::wstring& commandLine)
	{
		//Check command line
		if (StringW_IsNullOrWhitespace(commandLine))
		{
			return;
		}

		//Remove executable path
		int endIndex = 0;
		bool inQuotes = false;
		for (wchar_t commandChar : commandLine)
		{
			if (commandChar == '"')
			{
				inQuotes = !inQuotes;
			}
			else if (!inQuotes && commandChar == ' ')
			{
				break;
			}
			endIndex++;
		}
		commandLine = commandLine.substr(endIndex);

		//Trim command line
		StringW_Trim(commandLine);
	}

	std::wstring Process_GetApplicationParameter(HANDLE hProcess, __PROCESS_PARAMETER_OPTIONS pOption)
	{
		std::wstring parameterString = L"";
		BOOL target32bit = FALSE;
		BOOL current32bit = FALSE;
		IsWow64Process(hProcess, &target32bit);
		IsWow64Process(GetCurrentProcess(), &current32bit);
		if (current32bit && target32bit)
		{
			parameterString = GetApplicationParameter32(hProcess, pOption);
		}
		else if (current32bit && !target32bit)
		{
			parameterString = GetApplicationParameter64(hProcess, pOption);
		}
		else if (!current32bit && target32bit)
		{
			parameterString = GetApplicationParameterWOW64(hProcess, pOption);
		}
		else if (!current32bit && !target32bit)
		{
			parameterString = GetApplicationParameter32(hProcess, pOption);
		}
		else
		{
			std::wcout << "GetApplicationParameter unknown architecture." << std::endl;
		}

		//Remove executable path from commandline
		if (pOption == CommandLine)
		{
			CommandLine_RemoveExePath(parameterString);
		}

		return parameterString;
	}
}