// Copyright to Avram Traian. 2022 - 2022.
// File created on August 30 2022.

#pragma once

#include "Core/CoreTypes.h"
#include "KeyCodes.h"

namespace Leaf {

	enum class KeyState : uint8
	{
		Uknown = 0,

		Pressed,
		Held,
		Released,
		None
	};

	enum class CursorMode : uint8
	{
		Uknown = 0,

		Normal,
		Hidden,
		Locked
	};

	class LEAF_API Input
	{
	public:
		static bool Initialize();

		static void Shutdown();

	public:
		static bool IsKeyDown(Key key);
		static bool IsKeyPressed(Key key);
		static bool IsKeyHeld(Key key);

		static bool IsKeyUp(Key key);
		static bool IsKeyReleased(Key key);

		static int32 GetMouseX();
		static int32 GetMouseY();

		static int32 GetVirtualMouseX();
		static int32 GetVirtualMouseY();

		static int32 GetMouseDeltaX();
		static int32 GetMouseDeltaY();

		static void SetCursorMode(CursorMode mode);

		static CursorMode GetCursorMode();

	private:
		static void Tick();

		static void Internal_SetKeyState(Key key, KeyState state);

		static void Internal_SetMousePosition(int32 mouse_x, int32 mouse_y);

	private:
		friend class Application;
	};

}