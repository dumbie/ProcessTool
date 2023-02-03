#pragma once
#include "includes.cpp"
#include "processclose.cpp"
#include "processlaunch.cpp"
#include "processrestart.cpp"

namespace
{
	void Console_WaitForInput()
	{
		std::cout << '\n' << "Press any key to close this window . . .";
		int readKey = _getch();
	}

	int Console_Startup(int argc, wchar_t* argv[])
	{
		std::wcout << L"Welcome to the process tool." << std::endl;

		////Debug launch arguments (Launch executable)
		//argc = 6;
		//argv = new wchar_t* [6];
		//argv[1] = String_Convert_to_LPWSTR("-normal");
		//argv[2] = String_Convert_to_LPWSTR("-nouiaccess");
		//argv[3] = String_Convert_to_LPWSTR("-exepath=""\"C:\\Windows\\System32\\Notepad.exe""\"");
		//argv[4] = String_Convert_to_LPWSTR("-workpath=""\"E:\\""\"");
		//argv[5] = String_Convert_to_LPWSTR("-args=""\"textfile.txt""\"");

		////Debug launch arguments (Launch Windows Store)
		//argc = 3;
		//argv = new wchar_t* [3];
		//argv[1] = String_Convert_to_LPWSTR("-uwp=Microsoft.WindowsNotepad_8wekyb3d8bbwe!App");
		//argv[2] = String_Convert_to_LPWSTR("-args=""\"Textfile.txt""\"");

		////Debug launch arguments (Close process)
		//argc = 3;
		//argv = new wchar_t* [3];
		//argv[1] = String_Convert_to_LPWSTR("-close");
		//argv[2] = String_Convert_to_LPWSTR("-pid=1000");
		//argv[2] = String_Convert_to_LPWSTR("-pname=Notepad.exe");
		//argv[2] = String_Convert_to_LPWSTR("-pname=Microsoft.WindowsNotepad_8wekyb3d8bbwe!App");

		////Debug launch arguments (Restart process)
		//argc = 4;
		//argv = new wchar_t* [4];
		//argv[1] = String_Convert_to_LPWSTR("-restart");
		//argv[2] = String_Convert_to_LPWSTR("-pid=1000");
		//argv[3] = String_Convert_to_LPWSTR("-skipargs");

		//Check launch arguments
		if (argc < 2)
		{
			std::wcout << L"Command line arguments:" << std::endl;
			std::wcout << L"Launch executable: ProcessTool -exepath=""\"C:\\Windows\\System32\\Notepad.exe""\" [opt: -workpath=""\"C:\\Windows\\System32""\"] [opt: -args=""\"Textfile.txt""\"] [opt: -normal or -admin] [opt: -nouiaccess]" << std::endl;
			std::wcout << L"Launch Windows Store: ProcessTool -uwp=""\"Microsoft.WindowsNotepad_8wekyb3d8bbwe!App""\" [opt: -args=""\"Textfile.txt""\"]" << std::endl;
			std::wcout << L"Close: ProcessTool -close -pid=1000 or -pname=""\"Notepad.exe""\" or -pname=""\"Microsoft.WindowsNotepad_8wekyb3d8bbwe!App""\"" << std::endl;
			std::wcout << L"Restart: ProcessTool -restart -pid=1000 [opt: -skipargs]" << std::endl;
			std::wcout << L"Show: ProcessTool -show -pid=1000" << std::endl;
			Console_WaitForInput();
			return 0;
		}

		//Filter launch arguments
		std::wstring argumentUWPAppId = L"";
		std::wstring argumentExePath = L"";
		std::wstring argumentWorkPath = L"";
		std::wstring argumentArgs = L"";
		std::wstring argumentpId = L"";
		std::wstring argumentpName = L"";
		BOOL argumentWait = FALSE;
		BOOL argumentUWP = FALSE;
		BOOL argumentNormal = FALSE;
		BOOL argumentAdmin = FALSE;
		BOOL argumentNoUiAccess = FALSE;
		BOOL argumentClose = FALSE;
		BOOL argumentRestart = FALSE;
		BOOL argumentSkipArgs = FALSE;
		for (int i = 1; i < argc; i++)
		{
			std::wstring argument = std::wstring(argv[i]);
			std::wcout << L"Tool argument: " << argument << std::endl;
			StringW_ToLower(argument);
			StringW_Trim(argument);

			if (std::wstring(argument).starts_with(L"-exepath="))
			{
				argumentExePath = argument;
				StringW_Replace(argumentExePath, L"-exepath=", L"");
				StringW_Replace(argumentExePath, L"\"", L"");
			}

			if (std::wstring(argument).starts_with(L"-workpath="))
			{
				argumentWorkPath = argument;
				StringW_Replace(argumentWorkPath, L"-workpath=", L"");
				StringW_Replace(argumentWorkPath, L"\"", L"");
			}

			if (std::wstring(argument).starts_with(L"-args="))
			{
				argumentArgs = argument;
				StringW_Replace(argumentArgs, L"-args=", L"");
				StringW_Replace(argumentArgs, L"\"", L"");
			}

			if (std::wstring(argument).starts_with(L"-pid="))
			{
				argumentpId = argument;
				StringW_Replace(argumentpId, L"-pid=", L"");
				StringW_Replace(argumentpId, L"\"", L"");
			}

			if (std::wstring(argument).starts_with(L"-pname="))
			{
				argumentpName = argument;
				StringW_Replace(argumentpName, L"-pname=", L"");
				StringW_Replace(argumentpName, L"\"", L"");
			}

			if (std::wstring(argument).starts_with(L"-uwp="))
			{
				argumentUWP = TRUE;
				argumentUWPAppId = argument;
				StringW_Replace(argumentUWPAppId, L"-uwp=", L"");
				StringW_Replace(argumentUWPAppId, L"\"", L"");
			}

			if (std::wstring(argument).starts_with(L"-normal"))
			{
				if (!argumentAdmin)
				{
					argumentNormal = TRUE;
				}
			}

			if (std::wstring(argument).starts_with(L"-admin"))
			{
				if (!argumentNormal)
				{
					argumentAdmin = TRUE;
				}
			}

			if (std::wstring(argument).starts_with(L"-nouiaccess"))
			{
				argumentNoUiAccess = TRUE;
			}

			if (std::wstring(argument).starts_with(L"-close"))
			{
				argumentClose = TRUE;
			}

			if (std::wstring(argument).starts_with(L"-restart"))
			{
				argumentRestart = TRUE;
			}

			if (std::wstring(argument).starts_with(L"-skipargs"))
			{
				argumentSkipArgs = TRUE;
			}

			if (std::wstring(argument).starts_with(L"-wait"))
			{
				argumentWait = TRUE;
			}
		}

		//Update tool access status
		Process_ToolUpdateAccessStatus();

		//Check launch arguments
		if (argumentClose)
		{
			if (!StringW_IsNullOrWhitespace(argumentpId))
			{
				Close_ProcessId(StringW_Convert_to_DWORD(argumentpId));
			}
			else if (!StringW_IsNullOrWhitespace(argumentpName))
			{
				Close_ProcessesName(argumentpName);
			}
		}
		else if (argumentRestart)
		{
			if (!StringW_IsNullOrWhitespace(argumentpId))
			{
				Restart_ProcessId(StringW_Convert_to_DWORD(argumentpId), argumentSkipArgs);
			}
		}
		else
		{
			if (argumentUWP)
			{
				if (StringW_IsNullOrWhitespace(argumentUWPAppId))
				{
					std::wcout << L"Windows application id not set." << std::endl;
					return 0;
				}
				Launch_Uwp(argumentUWPAppId, argumentArgs);
			}
			else
			{
				if (StringW_IsNullOrWhitespace(argumentExePath))
				{
					std::wcout << L"Executable path not set." << std::endl;
					return 0;
				}
				Launch_Prepare(argumentExePath, argumentWorkPath, argumentArgs, argumentNormal, argumentAdmin, argumentNoUiAccess);
			}
		}

		//Wait for user key input
		if (argumentWait)
		{
			Console_WaitForInput();
		}

		//return processId
		return 1337;
	}
}

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
	return Console_Startup(argc, argv);
}