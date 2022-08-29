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

		static inline Pair<ConsoleColor, ConsoleColor> LogTypeConsoleColors[(uint8)Logger::LogType::MaxEnumValue] =
		{
			//                                    Foreground                 Background
			Pair<ConsoleColor, ConsoleColor>(ConsoleColor::White,       ConsoleColor::Black), // Unknown
			Pair<ConsoleColor, ConsoleColor>(ConsoleColor::Purple,      ConsoleColor::Black), // Debug
			Pair<ConsoleColor, ConsoleColor>(ConsoleColor::Gray,        ConsoleColor::Black), // Trace
			Pair<ConsoleColor, ConsoleColor>(ConsoleColor::Green,       ConsoleColor::Black), // Info
			Pair<ConsoleColor, ConsoleColor>(ConsoleColor::LightYellow, ConsoleColor::Black), // Warn
			Pair<ConsoleColor, ConsoleColor>(ConsoleColor::LightRed,    ConsoleColor::Black), // Error
			Pair<ConsoleColor, ConsoleColor>(ConsoleColor::LightWhite,  ConsoleColor::Red  )  // Fatal
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
		SystemTime sys_time;
		Platform::GetLocalSystemTime(sys_time);

		String result = String::Format(
			"[%{,2}:%{,2}:%{,2}][%{}][%{}]:%{} %{}\n",
			sys_time.Hour, sys_time.Minute, sys_time.Second,
			LoggerData::LogTypeToString_ANSI[(uint8)type], tag, LoggerData::LogTypeWhitespace_ANSI[(uint8)type], message
		);

		Platform::SetConsoleColor(LoggerData::LogTypeConsoleColors[(uint8)type].A, LoggerData::LogTypeConsoleColors[(uint8)type].B);
		Platform::SubmitTextToConsole_ANSI(result.ToView());
	}

}