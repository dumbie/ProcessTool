#pragma once
#include "includes.cpp"
#include "string.cpp"

namespace
{
	std::wstring Process_GetApplicationUserModelId(HANDLE hProcess)
	{
		UINT32 stringLength = 0;
		LONG result = GetApplicationUserModelId(hProcess, &stringLength, NULL);
		if (result != ERROR_INSUFFICIENT_BUFFER || result == APPMODEL_ERROR_NO_APPLICATION)
		{
			//std::wcout << "Failed to get ApplicationUserModelId for: " << hProcess << std::endl;
			return L"";
		}

		if (stringLength <= 0)
		{
			//std::wcout << "Failed to get ApplicationUserModelId for: " << hProcess << std::endl;
			return L"";
		}

		WCHAR* getString = new WCHAR[stringLength];
		result = GetApplicationUserModelId(hProcess, &stringLength, getString);
		if (result != ERROR_SUCCESS)
		{
			std::wcout << "Failed to get ApplicationUserModelId for: " << hProcess << std::endl;
			return L"";
		}

		return getString;
	}

	std::wstring Process_GetApplicationExePath(HANDLE hProcess)
	{
		DWORD getSize = MAX_PATH;
		WCHAR* getString = new WCHAR[getSize];
		if (!QueryFullProcessImageNameW(hProcess, 0, getString, &getSize))
		{
			std::wcout << "Failed to get ApplicationExePath for: " << hProcess << std::endl;
			return L"";
		}
		else
		{
			return getString;
		}
	}

	void CommandLine_RemoveExePath(std::wstring& commandLine)
	{
		if (StringW_IsNullOrWhitespace(commandLine))
		{
			return;
		}

		BOOL inQuotes = FALSE;
		const wchar_t* commandLineConst = commandLine.c_str();
		while (*commandLineConst)
		{
			if (*commandLineConst == L'"')
			{
				inQuotes = !inQuotes;
			}
			else if (!inQuotes && *commandLineConst == L' ')
			{
				commandLine = commandLineConst + 1;
				return;
			}
			commandLineConst = CharNextW(commandLineConst);
		}
		commandLine = commandLineConst;
	}
}