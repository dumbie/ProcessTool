#pragma once
#include "includes.h"
#include "processwindow.cpp"
#include "processdetails.cpp"
#include "processclose.cpp"
#include "processmulti.cpp"

namespace
{
	BOOL Show_ProcessIdHwnd(DWORD processId, HWND processHwnd)
	{
		std::wcout << L"Showing process by id and window handle: " << processId << "/" << processHwnd << std::endl;

		//Close open Windows prompts
		Close_ProcessName(L"consent.exe");
		Close_ProcessName(L"fondue.exe");

		//Get current window placement
		WINDOWPLACEMENT windowPlacement{};
		GetWindowPlacement(processHwnd, &windowPlacement);

		//Check current window placement
		INT windowShowCommand = SW_RESTORE;
		if (windowPlacement.flags == WPF_RESTORETOMAXIMIZED)
		{
			windowShowCommand = SW_SHOWMAXIMIZED;
		}

		for (int showCount = 0; showCount <= 2; showCount++)
		{
			//Allow changing window
			if (processId > 0)
			{
				AllowSetForegroundWindow(processId);
				Thread_Sleep(10);
			}

			//Show window async
			ShowWindowAsync(processHwnd, windowShowCommand);
			Thread_Sleep(10);

			//Show window normal
			ShowWindow(processHwnd, windowShowCommand);
			Thread_Sleep(10);

			//Bring window to top
			BringWindowToTop(processHwnd);
			Thread_Sleep(10);

			//Switch to the window
			SwitchToThisWindow(processHwnd, TRUE);
			Thread_Sleep(10);
		}

		return TRUE;
	}

	BOOL Show_ProcessHwnd(HWND processHwnd)
	{
		std::wcout << L"Showing process by window handle: " << processHwnd << std::endl;

		//Get process id from window handle
		DWORD foundProcessId = Window_GetProcessId(processHwnd);

		return Show_ProcessIdHwnd(foundProcessId, processHwnd);
	}

	BOOL Show_ProcessId(DWORD processId)
	{
		std::wcout << L"Showing process by id: " << processId << std::endl;

		//Get multi process
		__Process_Multi processDetails = ProcessMulti_GetFromProcessId(processId, PROCESS_QUERY_LIMITED_INFORMATION, TRUE);

		//Check window handle
		if (processDetails.MainWindowHandle == (HWND)0)
		{
			std::wcout << L"Failed showing process by id: " << processId << std::endl;
			return FALSE;
		}

		//Show process main window
		return Show_ProcessIdHwnd(processId, processDetails.MainWindowHandle);
	}
}