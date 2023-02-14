#pragma once
#include "includes.h"

namespace
{
	BOOL String_IsNullOrWhitespace(std::string targetString)
	{
		return targetString.empty() || std::all_of(targetString.begin(), targetString.end(), [](char chr) { return std::isspace(chr); });
	}

	BOOL StringW_IsNullOrWhitespace(std::wstring targetString)
	{
		return targetString.empty() || std::all_of(targetString.begin(), targetString.end(), [](wchar_t chr) { return std::isspace(chr); });
	}

	void String_ToLower(std::string& targetString)
	{
		for (char& chr : targetString) { chr = tolower(chr); }
	}

	void StringW_ToLower(std::wstring& targetString)
	{
		for (wchar_t& chr : targetString) { chr = tolower(chr); }
	}

	BOOL String_Contains(std::string targetString, std::string findString)
	{
		return targetString.find(findString) != std::string::npos;
	}

	BOOL StringW_Contains(std::wstring targetString, std::wstring findString)
	{
		return targetString.find(findString) != std::wstring::npos;
	}

	void String_Replace(std::string& targetString, std::string from, std::string to)
	{
		targetString = std::regex_replace(targetString, std::regex(from), to);
	}

	void StringW_Replace(std::wstring& targetString, std::wstring from, std::wstring to)
	{
		targetString = std::regex_replace(targetString, std::wregex(from), to);
	}

	void String_Trim(std::string& strimString)
	{
		const char* strimChar = " \t\n\r\f\v";
		strimString.erase(strimString.find_last_not_of(strimChar) + 1);
		strimString.erase(0, strimString.find_first_not_of(strimChar));
	}

	void StringW_Trim(std::wstring& strimString)
	{
		const wchar_t* strimChar = L" \t\n\r\f\v";
		strimString.erase(strimString.find_last_not_of(strimChar) + 1);
		strimString.erase(0, strimString.find_first_not_of(strimChar));
	}

	std::vector<std::string> String_Split(std::string targetString, std::string splitString, BOOL includeSplit)
	{
		//Find delimiters
		size_t lastPos = targetString.find_first_not_of(splitString, 0);
		size_t pos = targetString.find_first_of(splitString, lastPos);

		std::vector<std::string> stringVector;
		while (pos != std::string::npos || lastPos != std::string::npos)
		{
			//Add string to vector
			if (includeSplit)
			{
				size_t lastPosInc = lastPos - splitString.length();
				if (lastPosInc == std::string::npos) { lastPosInc = 0; }
				stringVector.push_back(targetString.substr(lastPosInc, pos - lastPosInc));
			}
			else
			{
				stringVector.push_back(targetString.substr(lastPos, pos - lastPos));
			}

			//Find delimiters
			lastPos = targetString.find_first_not_of(splitString, pos);
			pos = targetString.find_first_of(splitString, lastPos);
		}

		return stringVector;
	}

	std::vector<std::wstring> StringW_Split(std::wstring targetString, std::wstring splitString, BOOL includeSplit)
	{
		//Find delimiters
		size_t lastPos = targetString.find_first_not_of(splitString, 0);
		size_t pos = targetString.find_first_of(splitString, lastPos);

		std::vector<std::wstring> stringVector;
		while (pos != std::wstring::npos || lastPos != std::wstring::npos)
		{
			//Add string to vector
			if (includeSplit)
			{
				size_t lastPosInc = lastPos - splitString.length();
				if (lastPosInc == std::wstring::npos) { lastPosInc = 0; }
				stringVector.push_back(targetString.substr(lastPosInc, pos - lastPosInc));
			}
			else
			{
				stringVector.push_back(targetString.substr(lastPos, pos - lastPos));
			}

			//Find delimiters
			lastPos = targetString.find_first_not_of(splitString, pos);
			pos = targetString.find_first_of(splitString, lastPos);
		}

		return stringVector;
	}

	std::wstring String_Convert_to_StringW(std::string targetString)
	{
		//Check string length
		size_t requiredSize = 0;
		size_t stringLength = targetString.length();

		//Check required size
		mbstowcs_s(&requiredSize, NULL, 0, targetString.c_str(), stringLength);

		//Prepare the wchar array
		wchar_t* wCharArray = new wchar_t[requiredSize];

		//Convert to wchar array
		mbstowcs_s(&requiredSize, wCharArray, requiredSize, targetString.c_str(), stringLength);
		return wCharArray;
	}

	LPWSTR String_Convert_to_LPWSTR(std::string targetString)
	{
		//Check string length
		size_t requiredSize = 0;
		size_t stringLength = targetString.length();

		//Check required size
		mbstowcs_s(&requiredSize, NULL, 0, targetString.c_str(), stringLength);

		//Prepare the wchar array
		wchar_t* wCharArray = new wchar_t[requiredSize];

		//Convert to wchar array
		mbstowcs_s(&requiredSize, wCharArray, requiredSize, targetString.c_str(), stringLength);
		return wCharArray;
	}

	DWORD String_Convert_to_DWORD(std::string targetString)
	{
		DWORD doubleWord;
		std::stringstream stringStream;
		stringStream << targetString;
		stringStream >> doubleWord;
		return doubleWord;
	}

	DWORD StringW_Convert_to_DWORD(std::wstring targetString)
	{
		DWORD doubleWord;
		std::wstringstream stringStream;
		stringStream << targetString;
		stringStream >> doubleWord;
		return doubleWord;
	}

	std::string DWORD_Convert_to_String(DWORD targetDword)
	{
		std::string newString;
		std::stringstream stringStream;
		stringStream << targetDword;
		stringStream >> newString;
		return newString;
	}

	std::wstring DWORD_Convert_to_StringW(DWORD targetDword)
	{
		std::wstring newString;
		std::wstringstream stringStream;
		stringStream << targetDword;
		stringStream >> newString;
		return newString;
	}

	HWND String_Convert_to_HWND(std::string targetString)
	{
		return (HWND)stoll(targetString);
	}

	HWND StringW_Convert_to_HWND(std::wstring targetString)
	{
		return (HWND)stoll(targetString);
	}

	std::string StringW_Convert_to_String(std::wstring targetString)
	{
		//Check string length
		size_t requiredSize = 0;
		size_t stringLength = targetString.length();

		//Check required size
		wcstombs_s(&requiredSize, NULL, 0, targetString.c_str(), stringLength);

		//Prepare the char array
		char* charArray = new char[requiredSize];

		//Convert to char array
		wcstombs_s(&requiredSize, charArray, requiredSize, targetString.c_str(), stringLength);
		return charArray;
	}

	LPWSTR StringW_Convert_to_LPWSTR(std::wstring targetString)
	{
		//Check string length
		size_t requiredSize = 0;
		size_t stringLength = targetString.length();

		//Check required size
		wcstombs_s(&requiredSize, NULL, 0, targetString.c_str(), stringLength);

		//Prepare the char array
		char* charArray = new char[requiredSize];

		//Convert to char array
		wcstombs_s(&requiredSize, charArray, requiredSize, targetString.c_str(), stringLength);

		//Prepare the wchar array
		wchar_t* wCharArray = new wchar_t[requiredSize];

		//Convert to wchar array
		mbstowcs_s(&requiredSize, wCharArray, requiredSize, charArray, requiredSize);
		return wCharArray;
	}
}