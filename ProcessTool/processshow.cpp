#pragma once
#include "includes.h"
#include "processwindow.cpp"
#include "processdetails.cpp"

namespace
{
	BOOL Show_ProcessHwnd(DWORD processId, HWND processHwnd)
	{
		std::wcout << L"Showing process window handle: " << processHwnd << std::endl;

		//Fix Close open Windows prompts
		//Fix Close open start menu, cortana or search
		//Fix Close open Windows system menu

		//Get current window placement
		WINDOWPLACEMENT windowPlacement{};
		GetWindowPlacement(processHwnd, &windowPlacement);

		//Check current window placement
		INT windowShowCommand = SW_RESTORE;
		if (windowPlacement.flags == WPF_RESTORETOMAXIMIZED)
		{
			windowShowCommand = SW_SHOWMAXIMIZED;
		}

		//Allow changing window
		if (processId != 0)
		{
			AllowSetForegroundWindow(processId);
		}

		//Change window placement
		ShowWindowAsync(processHwnd, windowShowCommand);
		ShowWindow(processHwnd, windowShowCommand);

		//Bring window to top
		BringWindowToTop(processHwnd);

		//Switch to the window
		SwitchToThisWindow(processHwnd, true);

		return TRUE;
	}

	BOOL Show_ProcessId(DWORD processId)
	{
		std::wcout << L"Showing process by id: " << processId << std::endl;

		//Get process details
		__Process_Details processDetails = Process_GetDetails(processId, PROCESS_QUERY_LIMITED_INFORMATION, TRUE);
		
		//Check window handle
		if (processDetails.MainWindowHandle == (HWND)0)
		{
			std::wcout << L"Failed showing process by id: " << processId << std::endl;
			return FALSE;
		}

		//Show process main window
		return Show_ProcessHwnd(processId, processDetails.MainWindowHandle);
	}
}