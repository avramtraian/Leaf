// Copyright to Avram Traian. 2022 - 2022.
// File created on August 29 2022.

#pragma once

#include "Events/Event.h"

namespace Leaf {

	/** The window flags enum. */
	enum WindowFlagsEnum : uint32
	{
		/** Default. */
		WINDOW_FLAG_None = 0,

		/** The application lifetime is the same as the window lifetime. When the window closes, the application will shutdown. */
		WINDOW_FLAG_Primary = BIT(0),

		/** The window is in fullscreen mode. */
		WINDOW_FLAG_Fullscreen = BIT(1),

		/** The window will start maximized. After the window creation, this flag does nothing. */
		WINDOW_FLAG_StartMaximized = BIT(2),

		/** The window will start minimized. After the window creation, this flag does nothing. */
		WINDOW_FLAG_StartMinimized = BIT(3),

		/** The window is a fixed size. */
		WINDOW_FLAG_DisableResizing = BIT(4)
	};
	using WindowFlags = uint32;

	/** The window specification struct. */
	struct WindowSpecification
	{
		/** The window title. */
		StringUTF8 Title;

		/** The window available area width. */
		uint32 Width = 0;

		/** The window available area height. */
		uint32 Height = 0;

		/** The window available area top-left point X coordinate. */
		int32 PositionX = 0;

		/** The window available area top-left point Y coordinate. */
		int32 PositionY = 0;

		/** The function that will be called when a window event is received. */
		void(*EventCallback)(class Window*, Event&) = nullptr;

		/** The window flags. */
		WindowFlags Flags = WINDOW_FLAG_None;
	};

	namespace Renderer { class RenderingContext; }

	/** The Leaf Window class. */
	class LEAF_API Window
	{
	public:
		WindowSpecification& GetSpecification() { return m_Specification; }
		const WindowSpecification& GetSpecification() const { return m_Specification; }

		bool ShouldClose() const { return m_ShouldClose; }

		void* GetNativeHandle() const { return m_NativeHandle; }

		/**
		* Gets the available (client) width of the window.
		* The actual size of the window is this, plus the window left & right borders size.
		* 
		* @returns The window available width.
		*/
		uint32 GetWidth() const { return m_Specification.Width; }

		/**
		* Sets the available (client) width of the window.
		* The actual size of the window will be set to this, plus the left & right borders size.
		* 
		* @param new_width The new width of the available window area.
		*/
		void SetWidth(uint32 new_width);

		/**
		* Gets the available (client) height of the window.
		* The actual size of the window is this, plus the window top & bottom borders size.
		*
		* @returns The window available height.
		*/
		uint32 GetHeight() const { return m_Specification.Height; }

		/**
		* Sets the available (client) height of the window.
		* The actual size of the window will be set to this, plus the top & bottom borders size.
		*
		* @param new_height The new height of the available window area.
		*/
		void SetHeight(uint32 new_height);

		/**
		* Gets the available area top-left point coordinate on the X axis.
		* The actual window top-left point coordinate is this, minus the window left border size.
		* 
		* @returns The coordinate on the X axis of the top-left available window area point.
		*/
		int32 GetPositionX() const { return m_Specification.PositionX; }

		/**
		* Sets the coordinate on the X axis of the top-left available area point.
		* The actual window X coordinate will be this, minus the window left border size.
		* 
		* @param new_position_x The new position of the window available area.
		*/
		void SetPositionX(int32 new_position_x);

		/**
		* Gets the available area top-left point coordinate on the Y axis.
		* The actual window top-left point coordinate is this, minus the window top border size.
		*
		* @returns The coordinate on the Y axis of the top-left available window area point.
		*/
		int32 GetPositionY() const { return m_Specification.PositionY; }

		/**
		* Sets the coordinate on the Y axis of the top-left available area point.
		* The actual window Y coordinate will be this, minus the window top border size.
		*
		* @param new_position_y The new position of the window available area.
		*/
		void SetPositionY(int32 new_position_y);

		const StringUTF8& GetTitle() const { return m_Specification.Title; }
		void SetTitle(StringViewUTF8 new_title);

		bool IsPrimary() const { return m_Specification.Flags & WINDOW_FLAG_Primary; }
		void SetPrimary(bool primary);

		bool IsFullscreen() const { return m_Specification.Flags & WINDOW_FLAG_Fullscreen; }
		void SetFullsceen(bool fullscreen);

		void Maximize();
		void Minimize();

	private:
		Window(const WindowSpecification& specification);

	public:
		~Window();

		Window(const Window&) = delete;
		Window(Window&&) noexcept = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) noexcept = delete;

	private:
		void PumpWindowEvents();

		void ScheduleClose() { m_ShouldClose = true; }

	private:
		WindowSpecification m_Specification;

		void* m_NativeHandle;

		bool m_ShouldClose;

		void* m_OpaqueData;

		Ref<Renderer::RenderingContext> m_RenderingContext;

	private:
		friend class Application;
		friend class Unique<Window>;
	};

}