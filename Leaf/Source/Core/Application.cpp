// Copyright to Avram Traian. 2022 - 2022.
// File created on August 21 2022.

#include "Application.h"

#include "Platform/Platform.h"
#include "Memory.h"
#include "Logger.h"
#include "Engine/Input.h"
#include "Renderer/Renderer.h"

#include "Engine/Events/KeyEvents.h"
#include "Engine/Events/MouseEvents.h"
#include "Engine/Events/WindowEvents.h"

namespace Leaf {

	static bool s_RestartApplication = false;

	Application* Application::s_Instance = nullptr;

	bool ShouldRestartApplication()
	{
		return s_RestartApplication;
	}

	void SetRestartApplication(bool should_restart)
	{
		s_RestartApplication = should_restart;
	}

	bool InitializeCore()
	{
		if (!Platform::Initialize())
		{
			return false;
		}

		if (!Memory::Initialize())
		{
			Platform::Shutdown();
			return false;
		}

		if (!Logger::Initialize())
		{
			Memory::Shutdown();
			Platform::Shutdown();
			return false;
		}

		return true;
	}

	void ShutdownCore()
	{
		Logger::Shutdown();
		Memory::Shutdown();
		Platform::Shutdown();
	}

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
		, m_Running(false)
	{
		if (s_Instance)
			return;

		s_Instance = this;

		if (!Input::Initialize())
		{
			LF_CORE_FATAL("Input couldn't be initialized. Exiting...");
			return;
		}

		Renderer::Config.RenderingAPI = Renderer::API::Vulkan;
		if (!Renderer::Initialize())
		{
			LF_CORE_FATAL("Renderer couldn't be initialized. Exiting...");
			return;
		}

		CreateWindow(m_Specification.PrimaryWindowSpecification);
	}

	Application::~Application()
	{
		if (s_Instance != this)
			return;

		Renderer::Shutdown();

		Input::Shutdown();

		s_Instance = nullptr;
	}

	int32 Application::Run()
	{
		m_Running = true;

		while (m_Running)
		{
			for (Unique<Window>& window : m_WindowsRegistry)
			{
				window->PumpWindowEvents();
			}

			Input::Tick();

			for (int64 index = m_WindowsRegistry.Size() - 1; index >= 0; index--)
			{
				Unique<Window>& window = m_WindowsRegistry[index];

				if (window->ShouldClose())
				{
					if (window->IsPrimary())
					{
						m_Running = false;
						break;
					}
					m_WindowsRegistry.Remove(index);
				}
			}
			if (m_Running)
			{
				m_Running = !m_WindowsRegistry.IsEmpty();
			}
		}

		m_Running = false;
		m_WindowsRegistry.Clear();

		return 0;
	}

	Unique<Window>& Application::CreateWindow(const WindowSpecification& specification)
	{
		m_WindowsRegistry.Add(Unique<Window>::Create(specification));
		Unique<Window>& window = m_WindowsRegistry.Back();

		return window;
	}

	Unique<Window>& Application::GetWindow(void* native_handle)
	{
		for (Unique<Window>& window : m_WindowsRegistry)
		{
			if (window->GetNativeHandle() == native_handle)
				return window;
		}

		// The window doesn't exist.
		LF_ASSERT(false);
		return m_WindowsRegistry[0];
	}

	void Application::OnEvent(Window* window, Event& e)
	{
		EventDispatcher dispatcher(e, window);

		dispatcher.Dispatch<KeyPressedEvent>([](Window* window, const KeyPressedEvent& e) -> bool
		{
			if (Input::IsKeyDown(e.GetKeyCode()))
				Input::Internal_SetKeyState(e.GetKeyCode(), KeyState::Held);
			else
				Input::Internal_SetKeyState(e.GetKeyCode(), KeyState::Pressed);

			return false;
		});

		dispatcher.Dispatch<KeyReleasedEvent>([](Window* window, const KeyReleasedEvent& e) -> bool
		{
			Input::Internal_SetKeyState(e.GetKeyCode(), KeyState::Released);
			return false;
		});

		dispatcher.Dispatch<MouseMovedEvent>([](Window* window, const MouseMovedEvent& e) -> bool
		{
			Input::Internal_SetMousePosition(window->GetPositionX() + e.GetMouseX(), window->GetPositionY() + e.GetMouseY());
			return false;
		});

		dispatcher.Dispatch<MouseButtonDownEvent>([](Window* window, const MouseButtonDownEvent& e) -> bool
		{
			return false;
		});

		dispatcher.Dispatch<MouseButtonUpEvent>([](Window* window, const MouseButtonUpEvent& e) -> bool
		{
			return false;
		});

		dispatcher.Dispatch<WindowClosedEvent>([](Window* window, const WindowClosedEvent& e) -> bool
		{
			window->ScheduleClose();
			return false;
		});
	}

}