// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#include "Core/Platform/Platform.h"
#include "Core/Memory.h"

#include "WindowsHeaders.h"
#include <cstdlib>

namespace Leaf {

	struct WindowsPlatformData
	{
		HANDLE ProcessHandle;

		HANDLE ConsoleHandle;
		ConsoleColor CurrentConsoleColors[2];

		bool IsCursorVisible = true;
	};
	static WindowsPlatformData* s_PlatformData = nullptr;

	namespace Utils {

		static WORD ConsoleFlags(ConsoleColor foreground, ConsoleColor background)
		{
			return (WORD)foreground | ((WORD)background << 4);
		}

	}

	bool Platform::Initialize()
	{
		if (s_PlatformData)
			return false;

		s_PlatformData = (WindowsPlatformData*)malloc(sizeof(WindowsPlatformData));
		if (!s_PlatformData)
			return false;
		new (s_PlatformData) WindowsPlatformData();

		s_PlatformData->ProcessHandle = GetModuleHandle(NULL);

		s_PlatformData->ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		s_PlatformData->CurrentConsoleColors[0] = ConsoleColor::White;
		s_PlatformData->CurrentConsoleColors[1] = ConsoleColor::Black;

		if (s_PlatformData->ConsoleHandle != INVALID_HANDLE_VALUE)
			SetConsoleTextAttribute(s_PlatformData->ConsoleHandle, Utils::ConsoleFlags(s_PlatformData->CurrentConsoleColors[0], s_PlatformData->CurrentConsoleColors[1]));

		ShowCursor(TRUE);
		s_PlatformData->IsCursorVisible = true;

		return true;
	}

	void Platform::Shutdown()
	{
		if (!s_PlatformData)
			return;

		ShowCursor(TRUE);
		s_PlatformData->IsCursorVisible = true;

		if (s_PlatformData->ConsoleHandle != INVALID_HANDLE_VALUE)
			Platform::SetConsoleColor(ConsoleColor::White, ConsoleColor::Black);

		(*s_PlatformData).~WindowsPlatformData();
		free(s_PlatformData);
		s_PlatformData = nullptr;
	}

	void* Platform::GetProcessHandle()
	{
		return s_PlatformData->ProcessHandle;
	}

	void* Platform::Allocate(uint64 size)
	{
		return malloc(size);
	}

	void Platform::Free(void* block)
	{
		free(block);
	}

	void Platform::GetLocalSystemTime(SystemTime& out_calendar_time)
	{
		SYSTEMTIME system_time;
		GetLocalTime(&system_time);

		out_calendar_time.Year = system_time.wYear;
		out_calendar_time.Month = system_time.wMonth;
		out_calendar_time.Day = system_time.wDay;
		out_calendar_time.Hour = system_time.wHour;
		out_calendar_time.Minute = system_time.wMinute;
		out_calendar_time.Second = system_time.wSecond;
	}

	void Platform::SetConsoleColor(ConsoleColor foreground, ConsoleColor background)
	{
		if (s_PlatformData->ConsoleHandle == INVALID_HANDLE_VALUE)
			return;

		if (s_PlatformData->CurrentConsoleColors[0] == foreground && s_PlatformData->CurrentConsoleColors[1] == background)
			return;

		s_PlatformData->CurrentConsoleColors[0] = foreground;
		s_PlatformData->CurrentConsoleColors[1] = background;

		SetConsoleTextAttribute(s_PlatformData->ConsoleHandle, Utils::ConsoleFlags(s_PlatformData->CurrentConsoleColors[0], s_PlatformData->CurrentConsoleColors[1]));
	}

	void Platform::SubmitTextToConsole_UTF8(StringViewUTF8 text)
	{
		if (s_PlatformData->ConsoleHandle == INVALID_HANDLE_VALUE)
			return;

		WriteConsoleA(s_PlatformData->ConsoleHandle, text.CStr(), (DWORD)text.Length(), NULL, NULL);
	}

	void Platform::SetCursorVisibility(bool visibility)
	{
		if (s_PlatformData->IsCursorVisible == visibility)
			return;

		s_PlatformData->IsCursorVisible = visibility;
		ShowCursor(s_PlatformData->IsCursorVisible);
	}

	bool Platform::GetCursorVisibility()
	{
		return s_PlatformData->IsCursorVisible;
	}

	void Platform::GetMousePosition(int32& out_mouse_x, int32& out_mouse_y)
	{
		POINT mouse_pos;
		GetCursorPos(&mouse_pos);
		out_mouse_x = (int32)mouse_pos.x;
		out_mouse_y = (int32)mouse_pos.y;
	}

	void Platform::SetMousePosition(int32 mouse_x, int32 mouse_y)
	{
		SetCursorPos((int)mouse_x, (int)mouse_y);
	}

}