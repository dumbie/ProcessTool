#pragma once
#include "includes.h"
#include "strings.h"

namespace
{
	BOOL Check_PathShellCommand(std::wstring targetPath)
	{
		BOOL dividerPosition = targetPath.find(L":") > 1;
		BOOL shellCommand = StringW_Contains(targetPath, L":/") || StringW_Contains(targetPath, L":\\");
		return shellCommand && dividerPosition;
	}

	std::wstring Convert_ExePathToExeName(std::wstring targetPath)
	{
		std::filesystem::path filePath(targetPath);
		return filePath.filename();
	}
}