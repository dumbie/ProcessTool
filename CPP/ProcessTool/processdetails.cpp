#pragma once
#include "includes.h"
#include "defines.h"
#include "strings.h"
#include "processwindow.cpp"
#include "processparameter.cpp"
#include "processfunctions.cpp"

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

	std::wstring Process_GetApplicationExeName(HANDLE hProcess)
	{
		DWORD getSize = MAX_PATH;
		WCHAR* getString = new WCHAR[getSize];
		//K32GetModuleBaseNameW returns name but requires PROCESS_VM_READ
		if (!QueryFullProcessImageNameW(hProcess, 0, getString, &getSize))
		{
			std::wcout << "Failed to get ApplicationExeName for: " << hProcess << "/" << GetLastError() << std::endl;
			return L"";
		}
		else
		{
			return Convert_ExePathToFileNameWithExtension(getString);
		}
	}

	DWORD64 Process_GetApplicationRunTimeSec(HANDLE hProcess)
	{
		//Get create time
		FILETIME createTime;
		FILETIME exitTime;
		FILETIME kernelTime;
		FILETIME userTime;
		GetProcessTimes(hProcess, &createTime, &exitTime, &kernelTime, &userTime);

		//Get current time
		FILETIME currentTime;
		GetSystemTimeAsFileTime(&currentTime);

		//Calculate running time
		LARGE_INTEGER largeInteger{};
		largeInteger.LowPart = currentTime.dwLowDateTime - createTime.dwLowDateTime;
		largeInteger.HighPart = currentTime.dwHighDateTime - createTime.dwHighDateTime;

		DWORD64 returnSeconds = largeInteger.QuadPart;
		returnSeconds -= 11644473600LL;
		returnSeconds /= 10000000;
		return returnSeconds;
	}
}