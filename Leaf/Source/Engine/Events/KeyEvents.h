// Copyright to Avram Traian. 2022 - 2022.
// File created on August 30 2022.

#pragma once

#include "Event.h"
#include "Engine/KeyCodes.h"

namespace Leaf {

	class LEAF_API KeyPressedEvent : public Event
	{
	public:
		static EventType GetStaticEventType() { return EventType::KeyPressed; }

	public:
		KeyPressedEvent(Key key_code)
			: Event(GetStaticEventType())
			, m_KeyCode(key_code)
		{}

	public:
		Key GetKeyCode() const { return m_KeyCode; }

	private:
		Key m_KeyCode;
	};

	class LEAF_API KeyReleasedEvent : public Event
	{
	public:
		static EventType GetStaticEventType() { return EventType::KeyReleased; }

	public:
		KeyReleasedEvent(Key key_code)
			: Event(GetStaticEventType())
			, m_KeyCode(key_code)
		{}

	public:
		Key GetKeyCode() const { return m_KeyCode; }

	private:
		Key m_KeyCode;
	};

}