#pragma once
#include "includes.h"
#include "defines.h"

namespace
{
	//Structures
	struct __PEBWOW64
	{
		DWORD Reserved0;
		DWORD Reserved1;
		DWORD Reserved2;
		DWORD Reserved3;
		DWORD RtlUserProcessParameters;
	};

	struct __UNICODE_STRINGWOW64
	{
		USHORT Length;
		USHORT MaximumLength;
		DWORD Buffer;
	};

	struct __RTL_DRIVE_LETTER_CURDIRWOW64
	{
		USHORT Flags;
		USHORT Length;
		ULONG TimeStamp;
		__UNICODE_STRINGWOW64 DosPath;
	};

	struct __RTL_USER_PROCESS_PARAMETERSWOW64
	{
		ULONG MaximumLength;
		ULONG Length;
		ULONG Flags;
		ULONG DebugFlags;
		DWORD ConsoleHandle;
		ULONG ConsoleFlags;
		DWORD StandardInput;
		DWORD StandardOutput;
		DWORD StandardError;
		__UNICODE_STRINGWOW64 CurrentDirectory;
		DWORD CurrentDirectoryHandle;
		__UNICODE_STRINGWOW64 DllPath;
		__UNICODE_STRINGWOW64 ImagePathName;
		__UNICODE_STRINGWOW64 CommandLine;
		DWORD Environment;
		ULONG StartingX;
		ULONG StartingY;
		ULONG CountX;
		ULONG CountY;
		ULONG CountCharsX;
		ULONG CountCharsY;
		ULONG FillAttribute;
		ULONG WindowFlags;
		ULONG ShowWindowFlags;
		__UNICODE_STRINGWOW64 WindowTitle;
		__UNICODE_STRINGWOW64 DesktopInfo;
		__UNICODE_STRINGWOW64 ShellInfo;
		__UNICODE_STRINGWOW64 RuntimeData;
		__RTL_DRIVE_LETTER_CURDIRWOW64 CurrentDirectores[32];
		ULONG EnvironmentSize;
	};

	//Methods
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

		DWORD64 pebBaseAddress = NULL;
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

		return getString;
	}
}