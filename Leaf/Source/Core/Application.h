// Copyright to Avram Traian. 2022 - 2022.
// File created on August 21 2022.

#pragma once

#include "CoreTypes.h"

#include "Engine/Window.h"

namespace Leaf {

	LEAF_API bool ShouldRestartApplication();

	LEAF_API void SetRestartApplication(bool should_restart);

	LEAF_API bool InitializeCore();

	LEAF_API void ShutdownCore();

	struct CommandLineArguments
	{
		char** Arguments = 0;
		uint16 ArgumentsCount = 0;
	};

	struct ApplicationSpecification
	{
		/** The application name. */
		String Name;

		/** The command line arguments passed to the executable. */
		CommandLineArguments CommandArguments;

		/** The primary window specification. */
		WindowSpecification PrimaryWindowSpecification;
	};

	class LEAF_API Application
	{
	public:
		static Application* Get() { return s_Instance; }

	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application(Application&&) noexcept = delete;
		Application& operator=(Application&&) noexcept = delete;

	public:
		int32 Run();

		Unique<Window>& CreateWindow(const WindowSpecification& specification);
		Unique<Window>& GetWindow(void* native_handle);

		static void OnEvent(Window* window, Event& e);

	private:
		ApplicationSpecification m_Specification;

		bool m_Running;

		Vector<Unique<Window>> m_WindowsRegistry;

	private:
		static Application* s_Instance;
	};

}