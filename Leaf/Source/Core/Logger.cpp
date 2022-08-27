// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#include "Logger.h"

#include "Memory.h"
#include "Platform/Platform.h"

#include <iostream>

namespace Leaf {

	struct LoggerData
	{
		static inline const char* LogTypeToString_ANSI[(uint8)Logger::LogType::MaxEnumValue] =
		{
			"?????",
			"DEBUG",
			"TRACE",
			"INFO",
			"WARN",
			"ERROR",
			"FATAL" 
		};

		static inline const char* LogTypeWhitespace_ANSI[(uint8)Logger::LogType::MaxEnumValue] =
		{
			"" , // Unknown
			"" , // Debug
			"" , // Trace
			" ", // Info
			" ", // Warn
			"" , // Error
			""	 // Fatal
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

	void Logger::Submit(LogType type, StringView tag, StringView message)
	{
		uint8 hour = 17;
		uint8 minute = 3;
		uint8 second = 48;

		String result = String::Format(
			"[%{,2}:%{,2}:%{,2}][%{}][%{}]:%{} %{}\n",
			hour, minute, second,
			LoggerData::LogTypeToString_ANSI[(uint8)type], tag, LoggerData::LogTypeWhitespace_ANSI[(uint8)type], message
		);

		std::cout << result.CStr();
	}

}