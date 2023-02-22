#pragma once
#pragma warning(disable:5046)
#include "includes.h"
#include "defines.h"

namespace
{
	//processdetails
	std::wstring Process_GetApplicationUserModelId(HANDLE hProcess);

	//processfind
	std::vector<__Process_Handle> Find_ProcessesName(std::wstring processName, DWORD desiredAccess);
}