#pragma once
#include "includes.h"
#include "strings.h"

namespace
{
	void Thread_Sleep(int milliSeconds)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(milliSeconds));
	}

	BOOL Check_PathShellCommand(std::wstring targetPath)
	{
		BOOL dividerPosition = targetPath.find(L":") > 1;
		BOOL shellCommand = StringW_Contains(targetPath, L":/") || StringW_Contains(targetPath, L":\\");
		return shellCommand && dividerPosition;
	}

	std::wstring Convert_ExePathToFileNameWithExtension(std::wstring targetPath)
	{
		std::filesystem::path filePath(targetPath);
		return filePath.filename();
	}

	std::wstring Convert_ExePathToFileNameNoExtension(std::wstring targetPath)
	{
		std::filesystem::path filePath(targetPath);
		return filePath.filename().replace_extension();
	}

	std::wstring Convert_ExePathToWorkPath(std::wstring targetPath)
	{
		std::filesystem::path filePath(targetPath);
		return filePath.parent_path();
	}
}