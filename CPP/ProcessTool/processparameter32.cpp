#pragma once
#include "includes.h"
#include "defines.h"

namespace
{
	//Structures
	struct __PROCESS_BASIC_INFORMATION32
	{
		NTSTATUS ExitStatus;
		PVOID PebBaseAddress;
		PVOID AffinityMask;
		LONG BasePriority;
		PVOID UniqueProcessId;
		PVOID InheritedFromUniqueProcessId;
	};

	struct __PEB32
	{
		PVOID Reserved0;
		PVOID Reserved1;
		PVOID Reserved2;
		PVOID Reserved3;
		PVOID RtlUserProcessParameters;
	};

	struct __UNICODE_STRING32
	{
		USHORT Length;
		USHORT MaximumLength;
		PVOID Buffer;
	};

	struct __RTL_DRIVE_LETTER_CURDIR32
	{
		USHORT Flags;
		USHORT Length;
		ULONG TimeStamp;
		__UNICODE_STRING32 DosPath;
	};

	struct __RTL_USER_PROCESS_PARAMETERS32
	{
		ULONG MaximumLength;
		ULONG Length;
		ULONG Flags;
		ULONG DebugFlags;
		PVOID ConsoleHandle;
		ULONG ConsoleFlags;
		PVOID StandardInput;
		PVOID StandardOutput;
		PVOID StandardError;
		__UNICODE_STRING32 CurrentDirectory;
		PVOID CurrentDirectoryHandle;
		__UNICODE_STRING32 DllPath;
		__UNICODE_STRING32 ImagePathName;
		__UNICODE_STRING32 CommandLine;
		PVOID Environment;
		ULONG StartingX;
		ULONG StartingY;
		ULONG CountX;
		ULONG CountY;
		ULONG CountCharsX;
		ULONG CountCharsY;
		ULONG FillAttribute;
		ULONG WindowFlags;
		ULONG ShowWindowFlags;
		__UNICODE_STRING32 WindowTitle;
		__UNICODE_STRING32 DesktopInfo;
		__UNICODE_STRING32 ShellInfo;
		__UNICODE_STRING32 RuntimeData;
		__RTL_DRIVE_LETTER_CURDIR32 CurrentDirectores[32];
		ULONG EnvironmentSize;
	};

	//Methods
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

		return getString;
	}
}