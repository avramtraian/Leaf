// Copyright to Avram Traian. 2022 - 2022.
// File created on August 30 2022.

#pragma once

#include "Event.h"
#include "Engine/KeyCodes.h"

namespace Leaf {

	class LEAF_API MouseMovedEvent : public Event
	{
	public:
		static EventType GetStaticEventType() { return EventType::MouseMoved; }

	public:
		MouseMovedEvent(int32 mouse_x, int32 mouse_y)
			: Event(GetStaticEventType())
			, m_MouseX(mouse_x)
			, m_MouseY(mouse_y)
		{}

	public:
		int32 GetMouseX() const { return m_MouseX; }
		int32 GetMouseY() const { return m_MouseY; }

	private:
		int32 m_MouseX, m_MouseY;
	};

	class LEAF_API MouseButtonDownEvent : public Event
	{
	public:
		static EventType GetStaticEventType() { return EventType::MouseButtonDown; }

	public:
		MouseButtonDownEvent(MouseButton button)
			: Event(GetStaticEventType())
			, m_Button(button)
		{}

	public:
		MouseButton GetButton() const { return m_Button; }

	private:
		MouseButton m_Button;
	};

	class LEAF_API MouseButtonUpEvent : public Event
	{
	public:
		static EventType GetStaticEventType() { return EventType::MouseButtonUp; }

	public:
		MouseButtonUpEvent(MouseButton button)
			: Event(GetStaticEventType())
			, m_Button(button)
		{}

	public:
		MouseButton GetButton() const { return m_Button; }

	private:
		MouseButton m_Button;
	};

	class LEAF_API MouseWheelScrolledEvent : public Event
	{
	public:
		static EventType GetStaticEventType() { return EventType::MouseWheelScrolled; }

	public:
		MouseWheelScrolledEvent(int32 delta)
			: Event(GetStaticEventType())
			, m_Delta(delta)
		{}

	public:
		int32 GetDelta() const { return m_Delta; }

	private:
		int32 m_Delta;
	};

}