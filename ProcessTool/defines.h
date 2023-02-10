#pragma once
#include "includes.h"

namespace
{
	//Types
	typedef NTSTATUS(NTAPI* __NtQueryInformationProcess)(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength OPTIONAL);
	typedef NTSTATUS(NTAPI* __NtReadVirtualMemory32)(IN HANDLE ProcessHandle, IN PVOID BaseAddress, OUT PVOID Buffer, IN ULONG Size, OUT PULONG NumberOfBytesRead);
	typedef NTSTATUS(NTAPI* __NtReadVirtualMemory64)(IN HANDLE ProcessHandle, IN PVOID64 BaseAddress, OUT PVOID Buffer, IN ULONG64 Size, OUT PULONG64 NumberOfBytesRead);
	typedef NTSTATUS(NTAPI* __NtReadVirtualMemoryWOW64)(IN HANDLE ProcessHandle, IN DWORD64 BaseAddress, OUT PVOID Buffer, IN ULONG64 Size, OUT PULONG64 NumberOfBytesRead);
	typedef HANDLE(WINAPI* __GetProcessHandleFromHwnd)(IN HWND hWnd);

	//Enumerators
	enum __PROCESS_PARAMETER_OPTIONS
	{
		CurrentDirectoryPath,
		ImagePathName,
		CommandLine,
		DesktopInfo,
		Environment
	};

	enum __Process_Types
	{
		Win32,
		Win32Store,
		UWP
	};

	//Structures
	struct __EnumWindows_Params
	{
		DWORD targetProcessId;
		HWND foundWindowHandle;
	};

	struct __Process_Handle
	{
		DWORD Identifier;
		HANDLE ProcessHandle;
	};

	struct __Process_Details
	{
		DWORD Identifier;
		HANDLE ProcessHandle;
		__Process_Types ProcessType;
		HWND MainWindowHandle;
		std::wstring MainWindowClassName;
		std::wstring MainWindowTitle;
		std::wstring ApplicationUserModelId;
		std::wstring ExecutableName;
		std::wstring ExecutablePath;
		std::wstring WorkPath;
		std::wstring Argument;
	};

	struct __Process_Access
	{
		BOOL ProcessUiAccess = FALSE;
		BOOL ProcessAdminAccess = FALSE;
		BOOL ProcessElevation = FALSE;
		TOKEN_ELEVATION_TYPE ProcessElevationType = TokenElevationTypeDefault;
	};

	//Structures 32bit
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
		PVOID ProcessParameters; //RTL_USER_PROCESS_PARAMETERS
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

	//Structures 64bit
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
		PVOID64 ProcessParameters; //RTL_USER_PROCESS_PARAMETERS
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

	//Structures WOW64
	struct __PEBWOW64
	{
		DWORD Reserved0;
		DWORD Reserved1;
		DWORD Reserved2;
		DWORD Reserved3;
		DWORD ProcessParameters; //RTL_USER_PROCESS_PARAMETERS
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
}