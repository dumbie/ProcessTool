#pragma once
#include "includes.h"

namespace
{
	//Types
	typedef NTSTATUS(NTAPI* __NtQueryInformationProcess)(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength);
	typedef NTSTATUS(NTAPI* __NtReadVirtualMemory32)(IN HANDLE ProcessHandle, IN PVOID BaseAddress, OUT PVOID Buffer, IN ULONG NumberOfBytesToRead, OUT PULONG NumberOfBytesRead);
	typedef NTSTATUS(NTAPI* __NtReadVirtualMemory64)(IN HANDLE ProcessHandle, IN PVOID64 BaseAddress, OUT PVOID Buffer, IN ULONG64 NumberOfBytesToRead, OUT PULONG64 NumberOfBytesRead);
	typedef NTSTATUS(NTAPI* __NtReadVirtualMemoryWOW64)(IN HANDLE ProcessHandle, IN DWORD64 BaseAddress, OUT PVOID Buffer, IN ULONG64 NumberOfBytesToRead, OUT PULONG64 NumberOfBytesRead);
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

	struct __Process_Multi
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
}