#pragma once
#include "includes.h"
#include "processclose.cpp"
#include "processlaunch.cpp"
#include "processrestart.cpp"
#include "processshow.cpp"

namespace
{
	void Console_WaitForInput()
	{
		std::cout << "\nPress any key to close the process tool . . .";
		int readKey = _getch();
	}

	int Console_Startup(int argc, wchar_t* argv[])
	{
		std::wcout << L"Welcome to the process tool." << std::endl;

		//Check launch arguments
		if (argc < 2)
		{
			std::wcout << L"\nCommand line arguments:" << std::endl;
			std::wcout << L"Launch executable: ProcessTool -exepath=""\"C:\\Windows\\System32\\Notepad.exe""\" [opt: -workpath=""\"C:\\Windows\\System32""\"] [opt: -args=""\"Textfile.txt""\"] [opt: -normal or -admin] [opt: -allowuiaccess]" << std::endl;
			std::wcout << L"Launch Windows Store: ProcessTool -uwp=""\"Microsoft.WindowsNotepad_8wekyb3d8bbwe!App""\" [opt: -args=""\"Textfile.txt""\"]" << std::endl;
			std::wcout << L"Close: ProcessTool -close -pid=1000 or -hwnd=1000 or -pname=""\"Notepad.exe""\" or -pname=""\"Microsoft.WindowsNotepad_8wekyb3d8bbwe!App""\"" << std::endl;
			std::wcout << L"Restart: ProcessTool -restart -pid=1000 [opt: -withoutargs] [opt: -args=""\"Textfile.txt""\"]" << std::endl;
			std::wcout << L"Show: ProcessTool -show -pid=1000 or -hwnd=1000" << std::endl;
			Console_WaitForInput();
			return 0;
		}

		//Filter launch arguments
		std::wstring argumentUWPAppUserModelId = L"";
		std::wstring argumentExePath = L"";
		std::wstring argumentWorkPath = L"";
		std::wstring argumentArgs = L"";
		std::wstring argumentpId = L"";
		std::wstring argumentpName = L"";
		std::wstring argumentHWND = L"";
		BOOL argumentUWP = FALSE;
		BOOL argumentNormal = FALSE;
		BOOL argumentAdmin = FALSE;
		BOOL argumentAllowUiAccess = FALSE;
		BOOL argumentClose = FALSE;
		BOOL argumentShow = FALSE;
		BOOL argumentRestart = FALSE;
		BOOL argumentWithoutArgs = FALSE;
		for (int i = 1; i < argc; i++)
		{
			std::wstring argument = std::wstring(argv[i]);
			StringW_Trim(argument);
			std::wstring argumentLower = argument;
			StringW_ToLower(argumentLower);
			std::wcout << L"Tool argument: " << argument << std::endl;

			if (std::wstring(argumentLower).starts_with(L"-exepath="))
			{
				size_t cutLength = strlen("-exepath=");
				argumentExePath = argument.substr(cutLength);
			}

			else if (std::wstring(argumentLower).starts_with(L"-workpath="))
			{
				size_t cutLength = strlen("-workpath=");
				argumentWorkPath = argument.substr(cutLength);
			}

			else if (std::wstring(argumentLower).starts_with(L"-args="))
			{
				size_t cutLength = strlen("-args=");
				argumentArgs = argument.substr(cutLength);
			}

			else if (std::wstring(argumentLower).starts_with(L"-pid="))
			{
				size_t cutLength = strlen("-pid=");
				argumentpId = argument.substr(cutLength);
			}

			else if (std::wstring(argumentLower).starts_with(L"-pname="))
			{
				size_t cutLength = strlen("-pname=");
				argumentpName = argument.substr(cutLength);
			}

			else if (std::wstring(argumentLower).starts_with(L"-hwnd="))
			{
				size_t cutLength = strlen("-hwnd=");
				argumentHWND = argument.substr(cutLength);
			}

			else if (std::wstring(argumentLower).starts_with(L"-uwp="))
			{
				argumentUWP = TRUE;
				size_t cutLength = strlen("-uwp=");
				argumentUWPAppUserModelId = argument.substr(cutLength);
			}

			else if (std::wstring(argumentLower).starts_with(L"-normal"))
			{
				if (!argumentAdmin)
				{
					argumentNormal = TRUE;
				}
			}

			else if (std::wstring(argumentLower).starts_with(L"-admin"))
			{
				if (!argumentNormal)
				{
					argumentAdmin = TRUE;
				}
			}

			else if (std::wstring(argumentLower).starts_with(L"-allowuiaccess"))
			{
				argumentAllowUiAccess = TRUE;
			}

			else if (std::wstring(argumentLower).starts_with(L"-close"))
			{
				argumentClose = TRUE;
			}

			else if (std::wstring(argumentLower).starts_with(L"-show"))
			{
				argumentShow = TRUE;
			}

			else if (std::wstring(argumentLower).starts_with(L"-restart"))
			{
				argumentRestart = TRUE;
			}

			else if (std::wstring(argumentLower).starts_with(L"-withoutargs"))
			{
				argumentWithoutArgs = TRUE;
			}

			else if (std::wstring(argumentLower).starts_with(L"-debug"))
			{
				vToolDebugMode = TRUE;
			}

			else if (std::wstring(argumentLower).starts_with(L"-wait"))
			{
				vToolWaitClose = TRUE;
			}
		}

		//Update tool access status
		Process_ToolUpdateAccessStatus();

		//Check launch arguments
		int returnValue = 0;
		if (argumentClose)
		{
			if (!StringW_IsNullOrWhitespace(argumentpId))
			{
				returnValue = Close_ProcessTreeId(StringW_Convert_to_DWORD(argumentpId));
			}
			else if (!StringW_IsNullOrWhitespace(argumentpName))
			{
				returnValue = Close_ProcessName(argumentpName);
			}
			else if (!StringW_IsNullOrWhitespace(argumentHWND))
			{
				returnValue = Close_ProcessMessageHwnd(StringW_Convert_to_HWND(argumentHWND));
			}
		}
		else if (argumentRestart)
		{
			if (!StringW_IsNullOrWhitespace(argumentpId))
			{
				returnValue = Restart_ProcessId(StringW_Convert_to_DWORD(argumentpId), argumentArgs, argumentWithoutArgs);
			}
		}
		else if (argumentShow)
		{
			if (!StringW_IsNullOrWhitespace(argumentpId) && !StringW_IsNullOrWhitespace(argumentHWND))
			{
				returnValue = Show_ProcessIdHwnd(StringW_Convert_to_DWORD(argumentpId), StringW_Convert_to_HWND(argumentHWND));
			}
			else if (!StringW_IsNullOrWhitespace(argumentpId))
			{
				returnValue = Show_ProcessId(StringW_Convert_to_DWORD(argumentpId));
			}
			else if (!StringW_IsNullOrWhitespace(argumentHWND))
			{
				returnValue = Show_ProcessHwnd(StringW_Convert_to_HWND(argumentHWND));
			}
		}
		else
		{
			if (argumentUWP)
			{
				if (StringW_IsNullOrWhitespace(argumentUWPAppUserModelId))
				{
					std::wcout << L"Windows application user model id not set." << std::endl;
				}
				else
				{
					returnValue = Launch_Uwp(argumentUWPAppUserModelId, argumentArgs);
				}
			}
			else
			{
				if (StringW_IsNullOrWhitespace(argumentExePath))
				{
					std::wcout << L"Executable path not set." << std::endl;
				}
				else
				{
					returnValue = Launch_Prepare(argumentExePath, argumentWorkPath, argumentArgs, argumentNormal, argumentAdmin, argumentAllowUiAccess);
				}
			}
		}

		//Wait for user key input
		if (vToolWaitClose)
		{
			Console_WaitForInput();
		}

		//Return result value
		return returnValue;
	}
}

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
	return Console_Startup(argc, argv);
}