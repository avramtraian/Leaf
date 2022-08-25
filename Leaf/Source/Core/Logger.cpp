// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#include "Logger.h"

#include "Memory.h"
#include "Platform/Platform.h"

namespace Leaf {

	struct LoggerData
	{
		static constexpr const char* LogTypeToString_ANSI[(uint8)Logger::LogType::MaxEnumValue] =
		{
			"[?????]",
			"[DEBUG]",
			"[TRACE]",
			"[INFO]",
			"[WARN]",
			"[ERROR]",
			"[FATAL]"
		};
	};
	static LoggerData* s_LoggerData = nullptr;

	bool Logger::Initialize()
	{
		if (s_LoggerData)
			return false;

		s_LoggerData = lnew LoggerData();

		return true;
	}

	void Logger::Shutdown()
	{
		if (!s_LoggerData)
			return;

		ldelete s_LoggerData;
		s_LoggerData = nullptr;
	}

	void Logger::Submit(LogType type, const char* message, const char* tag)
	{

	}

}