//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#pragma once

#include <string>
#include <sstream>
#include "me_interface.h"

namespace me
{
	typedef uint32_t EntityID;
	constexpr int INVALID_ENTITY_ID = 0;
	constexpr size_t SHADER_NO_SETTINGS_BUFFER = 0;

	#define ME_SAFE_RELEASE(ptr) \
		if(ptr) \
		{ \
			(ptr)->Release(); \
			(ptr) = nullptr; \
		}

	#define ME_SAFE_DELETE(ptr) \
		if(ptr) \
		{ \
			delete (ptr); \
			(ptr) = nullptr; \
		}

	#define ME_ARRAY_COUNT(staticArray) (sizeof(staticArray) / sizeof((staticArray)[0]))

    //This macro can be used to disable the C4100 warning ('identifier' : unreferenced formal parameter) for specific parameters 
	#define ME_UNUSED(param) (void)param

    constexpr const char* COMMAND_ARG_SET = "1";
	constexpr const char* COMMAND_ARG_UNSET = "0";
	constexpr const char* COMMAND_ARG_APPLICATIONPATH = "applicationPath";
	ME_API std::string GetCommandLineArg(const std::string& argument);

	std::string ResolveURI(const std::string& uri);

	ME_API bool Failed(long hresult);

	ME_API bool ReadFileContents(const std::string& filePath, std::vector<uint8_t>& data, int sleepOnFail = 100);

	ME_API std::string UTF16ToUTF8(const std::wstring& utf16Str);
	ME_API std::wstring UTF8ToUTF16(const std::string& utf8Str);

	template<typename TOUT>
	void Split(const std::string &s, char delim, TOUT result) {
		std::stringstream ss;
		ss.str(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			*(result++) = item;
		}
	}
	ME_API bool StartsWith(const std::string& str, const std::string& strCompare);
	ME_API bool EndsWith(const std::string& str, const std::string& strCompare);
	ME_API std::string TrimWhitespace(const std::string& str);

	ME_API const char* StringFormat(const char* fmt, ...);

	ME_API bool AssertInternal(const char* file, const char* function, uint32_t line, bool& callAgain, const char* condition, const char* fmt, ...);

	enum class ME_LOGCategory
	{
		Info,
		Warning,
		Error
	};

	ME_API void LogInternal(const char* file, const char* function, uint32_t line, ME_LOGCategory category, const char* fmt, ...);

#define ME_ASSERT(condition) ME_ASSERTF(condition, "")

#ifdef TARGET_DEBUG
	#define ME_ASSERTF(condition, fmt, ...) \
		do \
		{ \
			if (!(condition)) \
			{ \
				static bool s_CallAgain = true; \
				if(s_CallAgain && me::AssertInternal(__FILE__, __FUNCTION__, __LINE__, s_CallAgain, #condition, fmt, ##__VA_ARGS__)) \
				{ \
					DebugBreak(); \
				} \
			} \
		} while (0)
#else
	#define ME_ASSERTF(condition, fmt, ...) \
		do \
		{ \
			(void)(condition); \
		} while(0)
#endif

	#define ME_STATIC_ASSERT(condition) static_assert(condition, "")

	#define ME_LOG(category, fmt, ...) \
		do \
		{ \
			me::LogInternal(__FILE__, __FUNCTION__, __LINE__, category, fmt, ##__VA_ARGS__); \
		} while(0)

	#define ME_LOG_INFO(fmt, ...) ME_LOG(me::ME_LOGCategory::Info, fmt, ##__VA_ARGS__)

	#define ME_LOG_WARNING(fmt, ...) ME_LOG(me::ME_LOGCategory::Warning, fmt, ##__VA_ARGS__)

	#define ME_LOG_ERROR(fmt, ...) \
		ME_LOG(me::ME_LOGCategory::Error, fmt, ##__VA_ARGS__); \
		ME_ASSERTF(false, fmt, ##__VA_ARGS__)

	template<class TField, class TFlag>
	void SetFlag(TField& field, TFlag flag) { field |= flag; }
	
	template<class TField, class TFlag>
	void ClearFlag(TField& field, TFlag flag) { field &= ~flag; }

	template<class TField, class TFlag>
	bool IsFlagSet(TField field, TFlag flag) { return (field & flag) != 0; }

	#define ME_COPY_NOT_ALLOWED(type) \
		type(const type& other) = delete; \
		type& operator=(const type& other) = delete;  // NOLINT(bugprone-macro-parentheses)

	#define ME_MOVE_NOT_ALLOWED(type) \
		type(const type&& other) = delete; \
		type& operator=(const type&& other) = delete;  // NOLINT(bugprone-macro-parentheses)

    #define ME_MOVE_COPY_NOT_ALLOWED(type)\
        ME_COPY_NOT_ALLOWED(type)\
        ME_MOVE_NOT_ALLOWED(type)

	//StopWatch is a wrapper around QueryPerformanceCounter which provides precision in micro second range.
	//GetTickCount64 only provides 10 - 16 ms precision which is by far not enough
	class StopWatch
	{
	public:
		ME_API StopWatch();
		ME_API float Measure();
	private:
		int64_t m_StartTime;
	};

	struct ScopedHandle
	{
		ScopedHandle() : m_Handle(INVALID_HANDLE_VALUE) {}
		explicit ScopedHandle(void* handle) : m_Handle(handle) {}
		~ScopedHandle()
		{
			Close();
		}
		bool IsValid() const { return m_Handle && m_Handle != INVALID_HANDLE_VALUE; }
		void Close()
		{
			if (IsValid())
			{
				CloseHandle(m_Handle);
				m_Handle = INVALID_HANDLE_VALUE;
			}
		}
		void Set(void* handle)
		{
			Close();
			m_Handle = handle;
		}
		void* GetHandle() const { return m_Handle; }
		operator void*() const { return m_Handle; }
	private:
		ME_MOVE_COPY_NOT_ALLOWED(ScopedHandle);
		void* m_Handle;
	};
};