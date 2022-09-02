// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "Core/CoreTypes.h"

namespace Leaf {

	struct SystemTime
	{
		uint16 Year = 0;
		uint16 Month = 0;
		uint16 Day = 0;
		uint16 Hour = 0;
		uint16 Minute = 0;
		uint16 Second = 0;
	};

	enum class ConsoleColor
	{
		Black    = 0,  Blue        = 1,  Green       = 2,  Aqua       = 3,
		Red      = 4,  Purple      = 5,  Yellow      = 6,  White      = 7,
		Gray     = 8,  LightBlue   = 9,  LightGreen  = 10, LightAqua  = 11,
		LightRed = 12, LightPurple = 13, LightYellow = 14, LightWhite = 15
	};

	class Platform
	{
	public:
		static bool Initialize();

		static void Shutdown();

	public:
		static void* GetProcessHandle();

	public:
		static void* Allocate(uint64 size);

		static void Free(void* block);

	public:
		static void GetLocalSystemTime(SystemTime& out_calendar_time);

	public:
		static void SetConsoleColor(ConsoleColor foreground, ConsoleColor background);

		static void SubmitTextToConsole_ANSI(StringView text);

	public:
		static void SetCursorVisibility(bool visibility);

		static bool GetCursorVisibility();

		static void GetMousePosition(int32& out_mouse_x, int32& out_mouse_y);

		static void SetMousePosition(int32 mouse_x, int32 mouse_y);
	};

}