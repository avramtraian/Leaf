// Copyright to Avram Traian. 2022 - 2022.
// File created on August 30 2022.

#pragma once

namespace Leaf {

	enum class EventType : uint8
	{
		Unknown = 0,

		KeyPressed,
		KeyReleased,

		MouseMoved,
		MouseButtonDown,
		MouseButtonUp,
		MouseWheelScrolled,

		WindowResized,
		WindowMinimized,
		WindowMoved,
		WindowTitleChanged,
		WindowClosed,

		MaxEnumValue
	};

	class LEAF_API Event
	{
	public:
		Event(EventType type)
			: m_Type(type)
			, m_IsHandled(false)
		{}

	public:
		EventType GetEventType() const { return m_Type; }

	private:
		EventType m_Type;
		bool m_IsHandled;
		
		friend class EventDispatcher;
	};

	class Window;

	class LEAF_API EventDispatcher
	{
	public:
		template<typename T>
		using PFN_Callback = bool(Window*, const T&);

	public:
		EventDispatcher(Event& e, Window* window)
			: m_Event(e)
			, m_Window(window)
		{}

	public:
		template<typename T>
		void Dispatch(PFN_Callback<T> callback)
		{
			if (m_Event.GetEventType() == T::GetStaticEventType() && !m_Event.m_IsHandled)
			{
				m_Event.m_IsHandled = callback(m_Window, (const T&)m_Event);
			}
		}

	private:
		Event& m_Event;
		Window* m_Window;
	};

}