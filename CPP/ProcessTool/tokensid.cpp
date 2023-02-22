#pragma once
#include "includes.h"

namespace
{
	PSID Sid_Create_WellKnownType(WELL_KNOWN_SID_TYPE sidType)
	{
		DWORD sidSize = SECURITY_MAX_SID_SIZE;
		PSID sidPointer = calloc(1, sidSize);
		if (!CreateWellKnownSid(sidType, NULL, sidPointer, &sidSize))
		{
			std::wcout << L"CreateWellKnownSid failed: " << GetLastError() << "/" << sidType << std::endl;
			return NULL;
		}
		else
		{
			std::wcout << L"CreateWellKnownSid success: " << sidType << "/" << sidPointer << std::endl;
			return sidPointer;
		}
	}

	PSID Sid_Create_AllAdminGroup()
	{
		PSID sidPointer;
		SID_IDENTIFIER_AUTHORITY sidIdentifier = { SECURITY_NT_AUTHORITY };
		if (!AllocateAndInitializeSid(&sidIdentifier, 8, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &sidPointer))
		{
			std::wcout << L"AllocateAndInitializeSid admin failed: " << GetLastError() << std::endl;
			return NULL;
		}
		else
		{
			std::wcout << L"AllocateAndInitializeSid admin success: " << sidPointer << std::endl;
			return sidPointer;
		}
	}

	PSID Sid_Create_AllUserGroup()
	{
		PSID sidPointer;
		SID_IDENTIFIER_AUTHORITY sidIdentifier = { SECURITY_NT_AUTHORITY };
		if (!AllocateAndInitializeSid(&sidIdentifier, 8, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS, 0, 0, 0, 0, 0, 0, &sidPointer))
		{
			std::wcout << L"AllocateAndInitializeSid user failed: " << GetLastError() << std::endl;
			return NULL;
		}
		else
		{
			std::wcout << L"AllocateAndInitializeSid user success: " << sidPointer << std::endl;
			return sidPointer;
		}
	}
}