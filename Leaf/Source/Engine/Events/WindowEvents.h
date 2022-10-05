// Copyright to Avram Traian. 2022 - 2022.
// File created on August 30 2022.

#pragma once

#include "Event.h"

namespace Leaf {

	class LEAF_API WindowResizedEvent : public Event
	{
	public:
		static EventType GetStaticEventType() { return EventType::WindowResized; }

	public:
		WindowResizedEvent(uint32 width, uint32 height)
			: Event(GetStaticEventType())
			, m_Width(width)
			, m_Height(height)
		{}

		uint32 GetWidth() const { return m_Width; }
		uint32 GetHeight() const { return m_Height; }

	private:
		uint32 m_Width, m_Height;
	};

	class LEAF_API WindowMinimizedEvent : public Event
	{
	public:
		static EventType GetStaticEventType() { return EventType::WindowMinimized; }

	public:
		WindowMinimizedEvent()
			: Event(GetStaticEventType())
		{}
	};

	class LEAF_API WindowMovedEvent : public Event
	{
	public:
		static EventType GetStaticEventType() { return EventType::WindowMoved; }

	public:
		WindowMovedEvent(int32 pos_x, int32 pos_y)
			: Event(GetStaticEventType())
			, m_PositionX(pos_x)
			, m_PositionY(pos_y)
		{}

		int32 GetPositionX() const { return m_PositionX; }
		int32 GetPositionY() const { return m_PositionY; }

	private:
		int32 m_PositionX, m_PositionY;
	};

	class LEAF_API WindowTitleChangedEvent : public Event
	{
	public:
		static EventType GetStaticEventType() { return EventType::WindowTitleChanged; }

	public:
		WindowTitleChangedEvent(StringViewUTF8 title)
			: Event(GetStaticEventType())
			, m_Title(title)
		{}

		StringViewUTF8 GetTitle() const { return m_Title; }

	private:
		StringViewUTF8 m_Title;
	};

	class LEAF_API WindowClosedEvent : public Event
	{
	public:
		static EventType GetStaticEventType() { return EventType::WindowClosed; }

	public:
		WindowClosedEvent()
			: Event(GetStaticEventType())
		{}
	};

}