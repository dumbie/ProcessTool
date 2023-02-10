#pragma once
#include "includes.h"
#include "tokensid.cpp"

namespace
{
	BOOL Token_Adjust_Privilege(HANDLE hToken, LPCWSTR privilegeName, BOOL enabled)
	{
		//SE_TCB_NAME
		//SE_ASSIGNPRIMARYTOKEN_NAME
		//SE_TIME_ZONE_NAME

		//Lookup token priviliges
		LUID luidPrivilege;
		if (!LookupPrivilegeValueW(NULL, privilegeName, &luidPrivilege))
		{
			std::wcout << L"LookupPrivilegeValueW failed: " << GetLastError() << "/" << privilegeName << std::endl;
			return FALSE;
		}

		//Adjust token priviliges
		TOKEN_PRIVILEGES tokenPrivileges{};
		tokenPrivileges.PrivilegeCount = 1;
		tokenPrivileges.Privileges[0].Luid = luidPrivilege;
		tokenPrivileges.Privileges[0].Attributes = enabled ? SE_PRIVILEGE_ENABLED : SE_PRIVILEGE_REMOVED;
		if (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, sizeof(TOKEN_PRIVILEGES), 0, 0))
		{
			std::wcout << L"AdjustTokenPrivileges failed: " << GetLastError() << "/" << privilegeName << std::endl;
			return FALSE;
		}
		else
		{
			std::wcout << L"AdjustTokenPrivileges success: " << privilegeName << std::endl;
			return TRUE;
		}
	}

	BOOL Token_Adjust_DisableUiAccess(HANDLE hToken)
	{
		//Disable token uiaccess
		DWORD enableUiAccess = FALSE;
		if (!SetTokenInformation(hToken, TokenUIAccess, &enableUiAccess, sizeof(DWORD)))
		{
			std::wcout << L"SetTokenInformation uiaccess failed: " << GetLastError() << "/" << enableUiAccess << std::endl;
			return FALSE;
		}
		else
		{
			std::wcout << L"SetTokenInformation uiaccess success: " << enableUiAccess << std::endl;
			return TRUE;
		}
	}

	BOOL Token_Adjust_IntegrityLevel(WELL_KNOWN_SID_TYPE sidType, HANDLE hToken)
	{
		//Set token integrity level 
		SID_AND_ATTRIBUTES sidAttributes{};
		sidAttributes.Sid = Sid_Create_WellKnownType(sidType);
		sidAttributes.Attributes = SE_GROUP_INTEGRITY | SE_GROUP_INTEGRITY_ENABLED;
		if (!SetTokenInformation(hToken, TokenIntegrityLevel, &sidAttributes, sizeof(sidAttributes)))
		{
			std::wcout << L"SetTokenInformation integrity failed: " << GetLastError() << "/" << sidType << std::endl;
			return FALSE;
		}
		else
		{
			std::wcout << L"SetTokenInformation integrity success: " << sidType << "/" << sidAttributes.Sid << std::endl;
			return TRUE;
		}
	}
}