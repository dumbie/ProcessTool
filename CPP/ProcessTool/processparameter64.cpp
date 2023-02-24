#pragma once
#include "includes.h"
#include "defines.h"

namespace
{
	//Structures
	struct __PROCESS_BASIC_INFORMATION64
	{
		NTSTATUS ExitStatus;
		PVOID64 PebBaseAddress;
		PVOID64 AffinityMask;
		LONG BasePriority;
		PVOID64 UniqueProcessId;
		PVOID64 InheritedFromUniqueProcessId;
	};

	struct __PEB64
	{
		PVOID64 Reserved0;
		PVOID64 Reserved1;
		PVOID64 Reserved2;
		PVOID64 Reserved3;
		PVOID64 RtlUserProcessParameters;
	};

	struct __UNICODE_STRING64
	{
		USHORT Length;
		USHORT MaximumLength;
		PVOID64 Buffer;
	};

	struct __RTL_DRIVE_LETTER_CURDIR64
	{
		USHORT Flags;
		USHORT Length;
		ULONG TimeStamp;
		__UNICODE_STRING64 DosPath;
	};

	struct __RTL_USER_PROCESS_PARAMETERS64
	{
		ULONG MaximumLength;
		ULONG Length;
		ULONG Flags;
		ULONG DebugFlags;
		PVOID64 ConsoleHandle;
		ULONG ConsoleFlags;
		PVOID64 StandardInput;
		PVOID64 StandardOutput;
		PVOID64 StandardError;
		__UNICODE_STRING64 CurrentDirectory;
		PVOID64 CurrentDirectoryHandle;
		__UNICODE_STRING64 DllPath;
		__UNICODE_STRING64 ImagePathName;
		__UNICODE_STRING64 CommandLine;
		PVOID64 Environment;
		ULONG StartingX;
		ULONG StartingY;
		ULONG CountX;
		ULONG CountY;
		ULONG CountCharsX;
		ULONG CountCharsY;
		ULONG FillAttribute;
		ULONG WindowFlags;
		ULONG ShowWindowFlags;
		__UNICODE_STRING64 WindowTitle;
		__UNICODE_STRING64 DesktopInfo;
		__UNICODE_STRING64 ShellInfo;
		__UNICODE_STRING64 RuntimeData;
		__RTL_DRIVE_LETTER_CURDIR64 CurrentDirectores[32];
		ULONG EnvironmentSize;
	};

	//Methods
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

		return getString;
	}
}