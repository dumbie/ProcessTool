#pragma once
#include "includes.cpp"

namespace
{
	BOOL Show_ProcessId(DWORD processId)
	{
		std::wcout << L"Showing process by id: " << processId << std::endl;
		return TRUE;
	}
}