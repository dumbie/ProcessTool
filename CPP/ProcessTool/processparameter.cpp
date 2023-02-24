#pragma once
#include "includes.h"
#include "defines.h"
#include "strings.h"

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

	std::wstring GetApplicationParameter32(HANDLE hProcess, __PROCESS_PARAMETER_OPTIONS pOption)
	{
		std::wcout << "GetApplicationParameter architecture 32" << std::endl;

		HMODULE hModule = GetModuleHandleW(L"ntdll.dll");
		if (hModule == NULL)
		{
			std::wcout << "Failed to get GetModuleHandleW for: " << hProcess << std::endl;
			return L"";
		}

		__NtQueryInformationProcess queryInformationProcess = (__NtQueryInformationProcess)GetProcAddress(hModule, "NtQueryInformationProcess");
		__NtReadVirtualMemory32 readVirtualMemory = (__NtReadVirtualMemory32)GetProcAddress(hModule, "NtReadVirtualMemory");
		if (queryInformationProcess == NULL || readVirtualMemory == NULL)
		{
			std::wcout << "Failed to get GetProcAddress for: " << hProcess << std::endl;
			return L"";
		}

		__PROCESS_BASIC_INFORMATION32 basicInformation{};
		NTSTATUS queryResult = queryInformationProcess(hProcess, ProcessBasicInformation, &basicInformation, sizeof(basicInformation), NULL);
		if (!NT_SUCCESS(queryResult) || basicInformation.PebBaseAddress == NULL)
		{
			std::wcout << "Failed to get ProcessBasicInformation for: " << hProcess << std::endl;
			return L"";
		}

		__PEB32 pebCopy{};
		NTSTATUS readResult = readVirtualMemory(hProcess, basicInformation.PebBaseAddress, &pebCopy, sizeof(pebCopy), NULL);
		if (!NT_SUCCESS(readResult))
		{
			std::wcout << "Failed to get Peb for: " << hProcess << std::endl;
			return L"";
		}

		__RTL_USER_PROCESS_PARAMETERS32 paramsCopy{};
		readResult = readVirtualMemory(hProcess, pebCopy.RtlUserProcessParameters, &paramsCopy, sizeof(paramsCopy), NULL);
		if (!NT_SUCCESS(readResult))
		{
			std::wcout << "Failed to get ProcessParameters for: " << hProcess << std::endl;
			return L"";
		}

		ULONG stringLength = NULL;
		PVOID stringBuffer = NULL;
		if (pOption == CurrentDirectoryPath)
		{
			stringLength = paramsCopy.CurrentDirectory.Length;
			stringBuffer = paramsCopy.CurrentDirectory.Buffer;
		}
		else if (pOption == ImagePathName)
		{
			stringLength = paramsCopy.ImagePathName.Length;
			stringBuffer = paramsCopy.ImagePathName.Buffer;
		}
		else if (pOption == DesktopInfo)
		{
			stringLength = paramsCopy.DesktopInfo.Length;
			stringBuffer = paramsCopy.DesktopInfo.Buffer;
		}
		else if (pOption == Environment)
		{
			stringLength = paramsCopy.EnvironmentSize;
			stringBuffer = paramsCopy.Environment;
		}
		else
		{
			stringLength = paramsCopy.CommandLine.Length;
			stringBuffer = paramsCopy.CommandLine.Buffer;
		}

		if (stringLength <= 0)
		{
			std::wcout << "Failed to get ParameterString length for: " << hProcess << std::endl;
			return L"";
		}

		WCHAR* getString = new WCHAR[stringLength]{};
		readResult = readVirtualMemory(hProcess, stringBuffer, getString, stringLength, NULL);
		if (!NT_SUCCESS(readResult))
		{
			std::wcout << "Failed to get ParameterString for: " << hProcess << std::endl;
			return L"";
		}

		if (pOption == CommandLine)
		{
			std::wstring returnString = getString;
			CommandLine_RemoveExePath(returnString);
			return returnString;
		}

		return getString;
	}

	std::wstring GetApplicationParameter64(HANDLE hProcess, __PROCESS_PARAMETER_OPTIONS pOption)
	{
		std::wcout << "GetApplicationParameter architecture 64" << std::endl;

		HMODULE hModule = GetModuleHandleW(L"ntdll.dll");
		if (hModule == NULL)
		{
			std::wcout << "Failed to get GetModuleHandleW for: " << hProcess << std::endl;
			return L"";
		}

		__NtQueryInformationProcess queryInformationProcess = (__NtQueryInformationProcess)GetProcAddress(hModule, "NtWow64QueryInformationProcess64");
		__NtReadVirtualMemory64 readVirtualMemory = (__NtReadVirtualMemory64)GetProcAddress(hModule, "NtWow64ReadVirtualMemory64");
		if (queryInformationProcess == NULL || readVirtualMemory == NULL)
		{
			std::wcout << "Failed to get GetProcAddress for: " << hProcess << std::endl;
			return L"";
		}

		__PROCESS_BASIC_INFORMATION64 basicInformation{};
		NTSTATUS queryResult = queryInformationProcess(hProcess, ProcessBasicInformation, &basicInformation, sizeof(basicInformation), NULL);
		if (!NT_SUCCESS(queryResult) || basicInformation.PebBaseAddress == NULL)
		{
			std::wcout << "Failed to get ProcessBasicInformation for: " << hProcess << std::endl;
			return L"";
		}

		__PEB64 pebCopy{};
		NTSTATUS readResult = readVirtualMemory(hProcess, basicInformation.PebBaseAddress, &pebCopy, sizeof(pebCopy), NULL);
		if (!NT_SUCCESS(readResult))
		{
			std::wcout << "Failed to get Peb for: " << hProcess << std::endl;
			return L"";
		}

		__RTL_USER_PROCESS_PARAMETERS64 paramsCopy{};
		readResult = readVirtualMemory(hProcess, pebCopy.RtlUserProcessParameters, &paramsCopy, sizeof(paramsCopy), NULL);
		if (!NT_SUCCESS(readResult))
		{
			std::wcout << "Failed to get ProcessParameters for: " << hProcess << std::endl;
			return L"";
		}

		ULONG stringLength = NULL;
		PVOID64 stringBuffer = NULL;
		if (pOption == CurrentDirectoryPath)
		{
			stringLength = paramsCopy.CurrentDirectory.Length;
			stringBuffer = paramsCopy.CurrentDirectory.Buffer;
		}
		else if (pOption == ImagePathName)
		{
			stringLength = paramsCopy.ImagePathName.Length;
			stringBuffer = paramsCopy.ImagePathName.Buffer;
		}
		else if (pOption == DesktopInfo)
		{
			stringLength = paramsCopy.DesktopInfo.Length;
			stringBuffer = paramsCopy.DesktopInfo.Buffer;
		}
		else if (pOption == Environment)
		{
			stringLength = paramsCopy.EnvironmentSize;
			stringBuffer = paramsCopy.Environment;
		}
		else
		{
			stringLength = paramsCopy.CommandLine.Length;
			stringBuffer = paramsCopy.CommandLine.Buffer;
		}

		if (stringLength <= 0)
		{
			std::wcout << "Failed to get ParameterString length for: " << hProcess << std::endl;
			return L"";
		}

		WCHAR* getString = new WCHAR[stringLength]{};
		readResult = readVirtualMemory(hProcess, stringBuffer, getString, stringLength, NULL);
		if (!NT_SUCCESS(readResult))
		{
			std::wcout << "Failed to get ParameterString for: " << hProcess << std::endl;
			return L"";
		}

		if (pOption == CommandLine)
		{
			std::wstring returnString = getString;
			CommandLine_RemoveExePath(returnString);
			return returnString;
		}

		return getString;
	}

	std::wstring GetApplicationParameterWOW64(HANDLE hProcess, __PROCESS_PARAMETER_OPTIONS pOption)
	{
		std::wcout << "GetApplicationParameter architecture WOW64" << std::endl;

		HMODULE hModule = GetModuleHandleW(L"ntdll.dll");
		if (hModule == NULL)
		{
			std::wcout << "Failed to get GetModuleHandleW for: " << hProcess << std::endl;
			return L"";
		}

		__NtQueryInformationProcess queryInformationProcess = (__NtQueryInformationProcess)GetProcAddress(hModule, "NtQueryInformationProcess");
		__NtReadVirtualMemoryWOW64 readVirtualMemory = (__NtReadVirtualMemoryWOW64)GetProcAddress(hModule, "NtReadVirtualMemory");
		if (queryInformationProcess == NULL || readVirtualMemory == NULL)
		{
			std::wcout << "Failed to get GetProcAddress for: " << hProcess << std::endl;
			return L"";
		}

		DWORD64 pebBaseAddress{};
		NTSTATUS queryResult = queryInformationProcess(hProcess, ProcessWow64Information, &pebBaseAddress, sizeof(pebBaseAddress), NULL);
		if (!NT_SUCCESS(queryResult))
		{
			std::wcout << "Failed to get PebBaseAddress for: " << hProcess << std::endl;
			return L"";
		}

		__PEBWOW64 pebCopy{};
		NTSTATUS readResult = readVirtualMemory(hProcess, pebBaseAddress, &pebCopy, sizeof(pebCopy), NULL);
		if (!NT_SUCCESS(readResult))
		{
			std::wcout << "Failed to get Peb for: " << hProcess << std::endl;
			return L"";
		}

		__RTL_USER_PROCESS_PARAMETERSWOW64 paramsCopy{};
		readResult = readVirtualMemory(hProcess, pebCopy.RtlUserProcessParameters, &paramsCopy, sizeof(paramsCopy), NULL);
		if (!NT_SUCCESS(readResult))
		{
			std::wcout << "Failed to get ProcessParameters for: " << hProcess << std::endl;
			return L"";
		}

		ULONG stringLength = NULL;
		DWORD64 stringBuffer = NULL;
		if (pOption == CurrentDirectoryPath)
		{
			stringLength = paramsCopy.CurrentDirectory.Length;
			stringBuffer = paramsCopy.CurrentDirectory.Buffer;
		}
		else if (pOption == ImagePathName)
		{
			stringLength = paramsCopy.ImagePathName.Length;
			stringBuffer = paramsCopy.ImagePathName.Buffer;
		}
		else if (pOption == DesktopInfo)
		{
			stringLength = paramsCopy.DesktopInfo.Length;
			stringBuffer = paramsCopy.DesktopInfo.Buffer;
		}
		else if (pOption == Environment)
		{
			stringLength = paramsCopy.EnvironmentSize;
			stringBuffer = paramsCopy.Environment;
		}
		else
		{
			stringLength = paramsCopy.CommandLine.Length;
			stringBuffer = paramsCopy.CommandLine.Buffer;
		}

		if (stringLength <= 0)
		{
			std::wcout << "Failed to get ParameterString length for: " << hProcess << std::endl;
			return L"";
		}

		WCHAR* getString = new WCHAR[stringLength]{};
		readResult = readVirtualMemory(hProcess, stringBuffer, getString, stringLength, NULL);
		if (!NT_SUCCESS(readResult))
		{
			std::wcout << "Failed to get ParameterString for: " << hProcess << std::endl;
			return L"";
		}

		if (pOption == CommandLine)
		{
			std::wstring returnString = getString;
			CommandLine_RemoveExePath(returnString);
			return returnString;
		}

		return getString;
	}

	std::wstring Process_GetApplicationParameter(HANDLE hProcess, __PROCESS_PARAMETER_OPTIONS pOption)
	{
		BOOL target32bit = FALSE;
		BOOL current32bit = FALSE;
		IsWow64Process(hProcess, &target32bit);
		IsWow64Process(GetCurrentProcess(), &current32bit);
		if (current32bit && target32bit)
		{
			return GetApplicationParameter32(hProcess, pOption);
		}
		else if (current32bit && !target32bit)
		{
			return GetApplicationParameter64(hProcess, pOption);
		}
		else if (!current32bit && target32bit)
		{
			return GetApplicationParameterWOW64(hProcess, pOption);
		}
		else if (!current32bit && !target32bit)
		{
			return GetApplicationParameter32(hProcess, pOption);
		}
		else
		{
			std::wcout << "GetApplicationParameter unknown architecture." << std::endl;
			return L"";
		}
	}
}