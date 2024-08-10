//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "util.h"

#include "windowsinc.h"

#include <iostream>
#include <ctime>
#include <algorithm>
#include <cctype>

namespace me
{
	ME_API std::string GetCommandLineArg(const std::string& argument)
	{
		using namespace std;
		static map<string, string> s_ParsedArguments = [](){
			map<string, string> args;
			args[COMMAND_ARG_APPLICATIONPATH] = UTF16ToUTF8(__wargv[0]);

	        for(int i=1;i<__argc;i++)
            {
				string arg = UTF16ToUTF8(__wargv[i]);
				if (StartsWith(arg, "-"))
				{
                    const size_t index = arg.find_first_of('=');
					if(index == string::npos)
						args[TrimWhitespace(arg.substr(1))] = COMMAND_ARG_SET;
					else
						args[TrimWhitespace(arg.substr(1, index-1))] = TrimWhitespace(arg.substr(index+1));
				}
            }
			return args;
		}();

        const auto it = s_ParsedArguments.find(argument);
		if (it == s_ParsedArguments.end())
		    return COMMAND_ARG_UNSET;
		else
		    return it->second;
	}

	std::string ResolveURI(const std::string& uri)
	{
		static std::string s_AssetPath = []() {
		    std::string assetPath = "assets/";
            const std::string arg = GetCommandLineArg("assetPath");
            if (arg != COMMAND_ARG_UNSET)
            {
	            assetPath = arg;
	            if (!EndsWith(assetPath, "/"))
		            assetPath.append("/");
            }
			return assetPath;
		}();

	    std::string path = uri;
		if(StartsWith(path, "assets://"))
		{
			path.replace(0, strlen("assets://"), s_AssetPath);
			return path;
		}

		if(!StartsWith(path, "me://"))
		{
			ME_LOG_ERROR("Path is not a valid uri: %s", uri.c_str());
		    return std::string();
		}

		return path;
	}

	bool Failed(long hresult)
	{
		if (FAILED(hresult))
		{
			LPSTR buffer;
			if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, static_cast<DWORD>(hresult), LANG_USER_DEFAULT, reinterpret_cast<LPSTR>(&buffer), 0, nullptr))
				return true;

			ME_LOG_ERROR(buffer);
			LocalFree(buffer);
			return true;
		}
		return false;
	}

	const char* StringFormat(const char* fmt, ...)
	{
		static char s_Buffer[4096];
		va_list args;
		va_start(args, fmt);
		vsprintf_s(s_Buffer, fmt, args);
		va_end(args);

		return s_Buffer;
	}

	bool AssertInternal(const char* file, const char* function, uint32_t line, bool& callAgain, const char* condition, const char* fmt, ...)
	{
		ME_UNUSED(file);

		char buffer[4096];
		va_list args;
		va_start(args, fmt);
		vsprintf_s(buffer, fmt, args);
		va_end(args);

		char message[4096];
		sprintf_s(message, "Condition: %s\n\n%s\n\nFile: %s\nFunction: %s\nLine: %u", condition, buffer, file, function, line);
        const int result = MessageBoxA(nullptr, message, "Assertion failed", MB_ABORTRETRYIGNORE | MB_ICONWARNING);
		switch (result)
		{
		case IDABORT:
			return true;
		case IDRETRY:
			return false;
		case IDIGNORE:
			callAgain = false;
			return false;
		default:
			return false;
		}
	}

	void LogInternal(const char* file, const char* function, uint32_t line, ME_LOGCategory category, const char* fmt, ...)
	{
		ME_UNUSED(file);

		char buffer[4096];
		va_list args;
		va_start(args, fmt);
		vsprintf_s(buffer, fmt, args);
		va_end(args);

		const char* categoryName = nullptr;
		switch (category)
		{
		case ME_LOGCategory::Info:
			categoryName = "Info";
			break;
		case ME_LOGCategory::Warning:
			categoryName = "Warning";
			break;
		case ME_LOGCategory::Error:
			categoryName = "Error";
			break;
		}

		time_t now = time(nullptr);
		tm localtm = {};
		localtime_s(&localtm, &now);
		

		char message[4096];
		sprintf_s(message, "%02d:%02d:%02d [%s]: %s | %s (%u)\n", localtm.tm_hour, localtm.tm_min, localtm.tm_sec, categoryName, buffer, function, line);

		OutputDebugStringA(message);
		std::cout << message;
	}

	bool ReadFileContents(const std::string& filePath, std::vector<uint8_t>& data, int sleepOnFail)
    {
	    ScopedHandle fileHandle(CreateFileW(UTF8ToUTF16(filePath).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
		if (!fileHandle.IsValid() && sleepOnFail >= 0)
		{
			Sleep(sleepOnFail);
			fileHandle.Set(CreateFileW(UTF8ToUTF16(filePath).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
		}

		if (!fileHandle.IsValid())
		{
			ME_LOG_ERROR("Cannot find file: %s", filePath.c_str());
			return false;
		}

        const DWORD size = GetFileSize(fileHandle, nullptr);
		data.resize(size);
		DWORD read;
		if (!ReadFile(fileHandle, data.data(), size, &read, nullptr) || size != read)
		{
			ME_LOG_ERROR("Cannot read file: %s", filePath.c_str());
			return false;
		}
		return true;
	}

	std::string UTF16ToUTF8(const std::wstring& utf16Str)
	{
		if (utf16Str.empty()) return std::string();
        const int size = WideCharToMultiByte(CP_UTF8, 0, &utf16Str[0], static_cast<int>(utf16Str.size()), nullptr, 0, nullptr, nullptr);
		std::string utf8Str(size, 0);
		WideCharToMultiByte(CP_UTF8, 0, &utf16Str[0], static_cast<int>(utf16Str.size()), &utf8Str[0], size, nullptr, nullptr);
		return utf8Str;
	}

	std::wstring UTF8ToUTF16(const std::string& utf8Str)
	{
		if (utf8Str.empty()) return std::wstring();
        const int size = MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], static_cast<int>(utf8Str.size()), nullptr, 0);
		std::wstring utf16Str(size, 0);
		MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], static_cast<int>(utf8Str.size()), &utf16Str[0], size);
		return utf16Str;
	}

	bool StartsWith(const std::string& str, const std::string& strCompare)
	{
		if (strCompare.size() > str.size()) return false;
		return std::equal(strCompare.begin(), strCompare.end(), str.begin());
	}

	bool EndsWith(const std::string& str, const std::string& strCompare)
	{
		if (strCompare.size() > str.size()) return false;
		return std::equal(strCompare.rbegin(), strCompare.rend(), str.rbegin());
	}

	std::string TrimWhitespace(const std::string& str)
	{
        const auto wsfront = std::find_if_not(str.begin(), str.end(), std::isspace);
        const auto wsback = std::find_if_not(str.rbegin(), str.rend(), std::isspace).base();
		return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
	}

	StopWatch::StopWatch()
	{
		LARGE_INTEGER startTime;
		startTime.QuadPart = 0;
		QueryPerformanceCounter(&startTime);
		m_StartTime = startTime.QuadPart;
	}

	float StopWatch::Measure()
	{
		LARGE_INTEGER stopTime;
		stopTime.QuadPart = 0;
		QueryPerformanceCounter(&stopTime);

		LARGE_INTEGER frequency;
		frequency.QuadPart = 0;
		QueryPerformanceFrequency(&frequency);

		const float elapsedTime = static_cast<float>(stopTime.QuadPart - m_StartTime) / frequency.QuadPart;
		m_StartTime = stopTime.QuadPart;
		return elapsedTime;
	}
}