// Copyright to Avram Traian. 2022 - 2022.
// File created on August 29 2022.

#if LF_PLATFORM_WINDOWS

#include "Engine/Window.h"

#include "Core/Platform/Platform.h"
#include "Core/Application.h"

#include "Engine/Events/KeyEvents.h"
#include "Engine/Events/MouseEvents.h"
#include "Engine/Events/WindowEvents.h"

#include "Renderer/Renderer.h"

#include "Core/Platform/Windows/WindowsHeaders.h"

namespace Leaf {

	static LRESULT WindowEventCallback(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	namespace Utils {

		constexpr const char* WindowClassName = "__Leaf_Window_Class__";

		static void RegisterWindowClass()
		{
			WNDCLASSEXA wnd_class = {};

			wnd_class.hInstance = (HINSTANCE)Platform::GetProcessHandle();
			wnd_class.lpfnWndProc = WindowEventCallback;
			wnd_class.hIcon = NULL;
			wnd_class.hIconSm = NULL;

			wnd_class.lpszClassName = WindowClassName;
			wnd_class.cbSize = sizeof(WNDCLASSEXA);
			wnd_class.hCursor = NULL;
			wnd_class.hbrBackground = NULL;
			wnd_class.cbWndExtra = 0;
			wnd_class.cbClsExtra = 0;
			wnd_class.lpszMenuName = NULL;
			wnd_class.style = 0;

			RegisterClassExA(&wnd_class);
		}

		static bool ValidateWindowFlags(WindowFlags flags)
		{
			if (flags & WINDOW_FLAG_Fullscreen)
			{
				if (flags & WINDOW_FLAG_StartMinimized)
					return false;

				return true;
			}
			else
			{
				if ((flags & WINDOW_FLAG_StartMaximized) && (flags & WINDOW_FLAG_StartMinimized))
					return false;

				return true;
			}
		}

		static void TranslateWindowFlags(WindowFlags flags, DWORD& out_style_flags, DWORD& out_ex_style_flags, int& out_show_mode)
		{
			out_ex_style_flags = 0;

			if (flags & WINDOW_FLAG_Fullscreen)
			{
				out_style_flags = WS_POPUP | WS_MAXIMIZE;
				out_show_mode = SW_SHOWMAXIMIZED;
			}
			else
			{
				out_style_flags = WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
				out_show_mode = SW_SHOW;

				if (!(flags & WINDOW_FLAG_DisableResizing))
				{
					out_style_flags |= WS_THICKFRAME | WS_MAXIMIZEBOX;
				}
				if (flags & WINDOW_FLAG_StartMaximized)
				{
					out_style_flags |= WS_MAXIMIZE;
					out_show_mode = SW_SHOWMAXIMIZED;
				}
				if (flags & WINDOW_FLAG_StartMinimized)
				{
					out_style_flags |= WS_MINIMIZE;
					out_show_mode = SW_SHOWMINIMIZED;
				}
			}
		}

		static Window* s_InCreationWindow = nullptr;

		static Window* GetWindow(HWND window_handle)
		{
			if (s_InCreationWindow)
				return s_InCreationWindow;

			return Application::Get()->GetWindow(window_handle).Get();
		}

	}

	struct WindowsWindowData
	{
		uint32 PaddingLeft = 0;
		uint32 PaddingRight = 0;
		uint32 PaddingTop = 0;
		uint32 PaddingBottom = 0;
	};

	Window::Window(const WindowSpecification& specification)
		: m_Specification(specification)
		, m_NativeHandle(NULL)
		, m_ShouldClose(false)
		, m_OpaqueData(nullptr)
	{
		m_OpaqueData = lnew WindowsWindowData();

		// First time a window is created, the window class is registered
		static bool registered_window_class = false;
		if (!registered_window_class)
		{
			Utils::RegisterWindowClass();
			registered_window_class = true;
		}

		// The specified flags are an invalid combination.
		LF_ASSERT_RETURN_VOID(Utils::ValidateWindowFlags(m_Specification.Flags));

		DWORD style_flags;
		DWORD ex_style_flags;
		int show_mode;
		Utils::TranslateWindowFlags(m_Specification.Flags, style_flags, ex_style_flags, show_mode);

		// An EventCallback function pointer MUST be provided.
		LF_ASSERT_RETURN_VOID(m_Specification.EventCallback);

		struct StackLifetime
		{
			StackLifetime(Window* w) { Utils::s_InCreationWindow = w; }
			~StackLifetime() { Utils::s_InCreationWindow = nullptr; }
		};
		StackLifetime in_creation_window(this);

		HWND window_hwnd = CreateWindowExA(
			ex_style_flags, Utils::WindowClassName,
			m_Specification.Title.CStr(),
			style_flags,
			m_Specification.PositionX, m_Specification.PositionY, m_Specification.Width, m_Specification.Height,
			NULL, NULL, (HINSTANCE)Platform::GetProcessHandle(), NULL);

		// The OS-call window creation failed.
		LF_ASSERT_RETURN_VOID(window_hwnd);
		m_NativeHandle = window_hwnd;

		ShowWindow((HWND)m_NativeHandle, show_mode);

		Renderer::RenderingContextSpecification rendering_context_specification;
		rendering_context_specification.Window = this;

		Renderer::CreateRenderingContext(m_RenderingContext, rendering_context_specification);

		RECT window_rect;
		GetWindowRect((HWND)m_NativeHandle, &window_rect);

		RECT client_rect;
		GetClientRect((HWND)m_NativeHandle, &client_rect);

		WindowsWindowData* win_data = (WindowsWindowData*)m_OpaqueData;
		win_data->PaddingLeft = ((window_rect.right - window_rect.left) - client_rect.right) / 2;
		win_data->PaddingRight = win_data->PaddingLeft;
		win_data->PaddingBottom = win_data->PaddingLeft;
		win_data->PaddingTop = ((window_rect.bottom - window_rect.top) - client_rect.bottom) - win_data->PaddingBottom;
	}

	Window::~Window()
	{
		ldelete m_OpaqueData;
		DestroyWindow((HWND)m_NativeHandle);
	}

	void Window::SetWidth(uint32 new_width)
	{
		WindowsWindowData* win_data = (WindowsWindowData*)m_OpaqueData;
		SetWindowPos((HWND)m_NativeHandle, NULL, 0, 0, new_width + (win_data->PaddingLeft + win_data->PaddingRight), m_Specification.Height + (win_data->PaddingTop + win_data->PaddingBottom), SWP_NOMOVE);
	}

	void Window::SetHeight(uint32 new_height)
	{
		WindowsWindowData* win_data = (WindowsWindowData*)m_OpaqueData;
		SetWindowPos((HWND)m_NativeHandle, NULL, 0, 0, m_Specification.Width + (win_data->PaddingLeft + win_data->PaddingRight), new_height + (win_data->PaddingTop + win_data->PaddingBottom), SWP_NOMOVE);
	}

	void Window::SetPositionX(int32 new_position_x)
	{
		WindowsWindowData* win_data = (WindowsWindowData*)m_OpaqueData;
		SetWindowPos((HWND)m_NativeHandle, NULL, new_position_x - win_data->PaddingLeft, m_Specification.PositionY - win_data->PaddingTop, 0, 0, SWP_NOSIZE);
	}

	void Window::SetPositionY(int32 new_position_y)
	{
		WindowsWindowData* win_data = (WindowsWindowData*)m_OpaqueData;
		SetWindowPos((HWND)m_NativeHandle, NULL, m_Specification.PositionX - win_data->PaddingLeft, new_position_y - win_data->PaddingTop, 0, 0, SWP_NOSIZE);
	}

	void Window::SetTitle(StringView new_title)
	{
		SetWindowTextA((HWND)m_NativeHandle, new_title.CStr());
	}

	void Window::SetPrimary(bool primary)
	{
		if (primary)
			m_Specification.Flags |= WINDOW_FLAG_Primary;
		else
			m_Specification.Flags &= (~WINDOW_FLAG_Primary);
	}

	void Window::SetFullsceen(bool fullscreen)
	{
		// The window is already fullscreen.
		if (fullscreen && (m_Specification.Flags & WINDOW_FLAG_Fullscreen))
			return;

		// The window is not fullscreen.
		if (!(fullscreen) && !(m_Specification.Flags & WINDOW_FLAG_Fullscreen))
			return;

		if (fullscreen)
			m_Specification.Flags |= WINDOW_FLAG_Fullscreen;
		else
			m_Specification.Flags &= (~WINDOW_FLAG_Fullscreen);

		LONG style_flags;
		int show_mode;

		if (m_Specification.Flags & WINDOW_FLAG_Fullscreen)
		{
			style_flags = WS_POPUP | WS_MAXIMIZE;
			show_mode = SW_SHOWMAXIMIZED;
		}
		else
		{
			style_flags = WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
			show_mode = SW_SHOW;

			if (!(m_Specification.Flags & WINDOW_FLAG_DisableResizing))
			{
				style_flags |= WS_THICKFRAME | WS_MAXIMIZEBOX;
			}
		}

		SetWindowLongA((HWND)m_NativeHandle, GWL_STYLE, style_flags);
		ShowWindow((HWND)m_NativeHandle, show_mode);

		RECT window_rect;
		GetWindowRect((HWND)m_NativeHandle, &window_rect);

		RECT client_rect;
		GetClientRect((HWND)m_NativeHandle, &client_rect);

		WindowsWindowData* win_data = (WindowsWindowData*)m_OpaqueData;
		win_data->PaddingLeft = ((window_rect.right - window_rect.left) - client_rect.right) / 2;
		win_data->PaddingRight = win_data->PaddingLeft;
		win_data->PaddingBottom = win_data->PaddingLeft;
		win_data->PaddingTop = ((window_rect.bottom - window_rect.top) - client_rect.bottom) - win_data->PaddingBottom;

		if (m_Specification.Flags & WINDOW_FLAG_Fullscreen)
		{
			HMONITOR monitor_handle = MonitorFromWindow((HWND)m_NativeHandle, MONITOR_DEFAULTTOPRIMARY);
			MONITORINFO monitor_info = {};
			monitor_info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfoA(monitor_handle, &monitor_info);

			SetPositionX(monitor_info.rcMonitor.left);
			SetPositionY(monitor_info.rcMonitor.top);
		}
	}

	void Window::Maximize()
	{
		ShowWindow((HWND)m_NativeHandle, SW_SHOWMAXIMIZED);
	}

	void Window::Minimize()
	{
		ShowWindow((HWND)m_NativeHandle, SW_SHOWMINIMIZED);
	}

	void Window::PumpWindowEvents()
	{
		MSG window_msg;
		while (PeekMessageA(&window_msg, (HWND)m_NativeHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&window_msg);
			DispatchMessageA(&window_msg);
		}
	}

	LRESULT WindowEventCallback(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		switch (Msg)
		{
			case WM_MOUSEMOVE:
			{
				POINTS pos = MAKEPOINTS(lParam);
				int32 position_x = (int32)pos.x;
				int32 position_y = (int32)pos.y;

				MouseMovedEvent e(position_x, position_y);

				Window* window = Utils::GetWindow(hWnd);
				window->GetSpecification().EventCallback(window, e);
				return 0;
			}

			case WM_LBUTTONDOWN:
			{
				MouseButtonDownEvent e(MouseButton::Left);

				Window* window = Utils::GetWindow(hWnd);
				window->GetSpecification().EventCallback(window, e);
				return 0;
			}
			case WM_LBUTTONUP:
			{
				MouseButtonUpEvent e(MouseButton::Left);

				Window* window = Utils::GetWindow(hWnd);
				window->GetSpecification().EventCallback(window, e);
				return 0;
			}

			case WM_RBUTTONDOWN:
			{
				MouseButtonDownEvent e(MouseButton::Right);

				Window* window = Utils::GetWindow(hWnd);
				window->GetSpecification().EventCallback(window, e);
				return 0;
			}
			case WM_RBUTTONUP:
			{
				MouseButtonUpEvent e(MouseButton::Right);

				Window* window = Utils::GetWindow(hWnd);
				window->GetSpecification().EventCallback(window, e);
				return 0;
			}

			case WM_MBUTTONDOWN:
			{
				MouseButtonDownEvent e(MouseButton::Middle);

				Window* window = Utils::GetWindow(hWnd);
				window->GetSpecification().EventCallback(window, e);
				return 0;
			}
			case WM_MBUTTONUP:
			{
				MouseButtonUpEvent e(MouseButton::Middle);

				Window* window = Utils::GetWindow(hWnd);
				window->GetSpecification().EventCallback(window, e);
				return 0;
			}

			case WM_MOUSEWHEEL:
			{
				int32 delta = GET_WHEEL_DELTA_WPARAM(wParam);

				MouseWheelScrolledEvent e(delta);

				Window* window = Utils::GetWindow(hWnd);
				window->GetSpecification().EventCallback(window, e);
				return 0;
			}

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			{
				Key key = (Key)wParam;

				KeyPressedEvent e(key);

				Window* window = Utils::GetWindow(hWnd);
				window->GetSpecification().EventCallback(window, e);
				return 0;
			}
			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				Key key = (Key)wParam;

				KeyReleasedEvent e(key);

				Window* window = Utils::GetWindow(hWnd);
				window->GetSpecification().EventCallback(window, e);
				return 0;
			}

			case WM_SIZE:
			{
				uint32 width  = (uint32)(LOWORD(lParam));
				uint32 height = (uint32)(HIWORD(lParam));

				Window* window = Utils::GetWindow(hWnd);

				if (width == 0 || height == 0)
				{
					WindowMinimizedEvent e;
					window->GetSpecification().EventCallback(window, e);
					return 0;
				}

				WindowResizedEvent e(width, height);

				
				window->GetSpecification().Width = width;
				window->GetSpecification().Height = height;
				window->GetSpecification().EventCallback(window, e);
				return 0;
			}

			case WM_MOVE:
			{
				POINTS pos = MAKEPOINTS(lParam);
				int32 position_x = (int32)pos.x;
				int32 position_y = (int32)pos.y;

				WindowMovedEvent e(position_x, position_y);

				Window* window = Utils::GetWindow(hWnd);
				window->GetSpecification().PositionX = position_x;
				window->GetSpecification().PositionY = position_y;
				window->GetSpecification().EventCallback(window, e);
				return 0;
			}

			case WM_CLOSE:
			{
				WindowClosedEvent e;

				Window* window = Utils::GetWindow(hWnd);
				window->GetSpecification().EventCallback(window, e);
				return 0;
			}
		}


		return DefWindowProcA(hWnd, Msg, wParam, lParam);
	}

}

#endif