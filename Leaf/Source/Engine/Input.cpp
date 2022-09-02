// Copyright to Avram Traian. 2022 - 2022.
// File created on August 30 2022.

#include "Input.h"
#include "Core/Platform/Platform.h"
#include "Core/Application.h"

namespace Leaf {

	struct InputData
	{
		Array<KeyState, 256> KeyStates;

		CursorMode CurrentCursorMode = CursorMode::Normal;

		int32 MouseX;
		int32 MouseY;
		int32 LastTickMouseX;
		int32 LastTickMouseY;

		int32 UnlockedMouseX;
		int32 UnlockedMouseY;
		int32 LastTickUnlockedMouseX;
		int32 LastTickUnlockedMouseY;

		int32 MouseDeltaX;
		int32 MouseDeltaY;
	};
	static InputData* s_InputData = nullptr;

	bool Input::Initialize()
	{
		if (s_InputData)
			return false;

		s_InputData = lnew InputData();

		for (SizeT index = 0; index < s_InputData->KeyStates.Size(); index++)
			s_InputData->KeyStates[index] = KeyState::Released;

		return true;
	}

	void Input::Shutdown()
	{
		if (!s_InputData)
			return;

		s_InputData->CurrentCursorMode = CursorMode::Normal;

		ldelete s_InputData;
		s_InputData = nullptr;
	}

	bool Input::IsKeyDown(Key key)
	{
		KeyState state = s_InputData->KeyStates[(uint8)key];
		return state == KeyState::Pressed || state == KeyState::Held;
	}

	bool Input::IsKeyPressed(Key key)
	{
		KeyState state = s_InputData->KeyStates[(uint8)key];
		return state == KeyState::Pressed;
	}

	bool Input::IsKeyHeld(Key key)
	{
		KeyState state = s_InputData->KeyStates[(uint8)key];
		return state == KeyState::Held;
	}

	bool Input::IsKeyUp(Key key)
	{
		KeyState state = s_InputData->KeyStates[(uint8)key];
		return state == KeyState::Released || state == KeyState::None;
	}

	bool Input::IsKeyReleased(Key key)
	{
		KeyState state = s_InputData->KeyStates[(uint8)key];
		return state == KeyState::Released;
	}

	int32 Input::GetMouseX()
	{
		return s_InputData->MouseX;
	}

	int32 Input::GetMouseY()
	{
		return s_InputData->MouseY;
	}

	int32 Input::GetVirtualMouseX()
	{
		return s_InputData->UnlockedMouseX;
	}

	int32 Input::GetVirtualMouseY()
	{
		return s_InputData->UnlockedMouseY;
	}

	int32 Input::GetMouseDeltaX()
	{
		return s_InputData->MouseDeltaX;
	}

	int32 Input::GetMouseDeltaY()
	{
		return s_InputData->MouseDeltaY;
	}

	void Input::SetCursorMode(CursorMode mode)
	{
		if (s_InputData->CurrentCursorMode == mode)
			return;

		s_InputData->CurrentCursorMode = mode;
		switch (s_InputData->CurrentCursorMode)
		{
			case CursorMode::Normal:
			{
				Platform::SetCursorVisibility(true);
				break;
			}
			case CursorMode::Hidden:
			{
				Platform::SetCursorVisibility(false);
				break;
			}
			case CursorMode::Locked:
			{
				s_InputData->UnlockedMouseX = s_InputData->MouseX;
				s_InputData->UnlockedMouseY = s_InputData->MouseY;

				Platform::SetCursorVisibility(false);
				break;
			}
		}
	}

	CursorMode Input::GetCursorMode()
	{
		return s_InputData->CurrentCursorMode;
	}

	void Input::Tick()
	{
		for (SizeT index = 0; index < s_InputData->KeyStates.Size(); index++)
		{
			if (s_InputData->KeyStates[index] == KeyState::Released)
				s_InputData->KeyStates[index] = KeyState::None;
			if (s_InputData->KeyStates[index] == KeyState::Pressed)
				s_InputData->KeyStates[index] = KeyState::Held;
		}

		if (s_InputData->CurrentCursorMode != CursorMode::Locked)
		{
			s_InputData->MouseDeltaX = s_InputData->MouseX - s_InputData->LastTickMouseX;
			s_InputData->MouseDeltaY = s_InputData->MouseY - s_InputData->LastTickMouseY;
		}
		else
		{
			s_InputData->MouseDeltaX = s_InputData->UnlockedMouseX - s_InputData->LastTickUnlockedMouseX;
			s_InputData->MouseDeltaY = s_InputData->UnlockedMouseY - s_InputData->LastTickUnlockedMouseY;
		}

		s_InputData->LastTickMouseX = s_InputData->MouseX;
		s_InputData->LastTickMouseY = s_InputData->MouseY;

		s_InputData->LastTickUnlockedMouseX = s_InputData->UnlockedMouseX;
		s_InputData->LastTickUnlockedMouseY = s_InputData->UnlockedMouseY;
	}

	void Input::Internal_SetKeyState(Key key, KeyState state)
	{
		s_InputData->KeyStates[(uint8)key] = state;
	}

	void Input::Internal_SetMousePosition(int32 mouse_x, int32 mouse_y)
	{
		if (mouse_x == s_InputData->MouseX && mouse_y == s_InputData->MouseY)
			return;

		if (s_InputData->CurrentCursorMode != CursorMode::Locked)
		{
			s_InputData->MouseX = mouse_x;
			s_InputData->MouseY = mouse_y;
		}
		else
		{
			int32 delta_x = mouse_x - s_InputData->MouseX;
			int32 delta_y = mouse_y - s_InputData->MouseY;

			s_InputData->UnlockedMouseX += delta_x;
			s_InputData->UnlockedMouseY += delta_y;

			Platform::SetMousePosition(s_InputData->MouseX, s_InputData->MouseY);
		}
	}

}