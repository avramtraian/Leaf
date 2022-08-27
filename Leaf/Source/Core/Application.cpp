// Copyright to Avram Traian. 2022 - 2022.
// File created on August 21 2022.

#include "Application.h"

#include "Platform/Platform.h"
#include "Memory.h"
#include "Logger.h"

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
	{
		if (s_Instance)
			return;

		s_Instance = this;
	}

	Application::~Application()
	{
		if (s_Instance != this)
			return;

		s_Instance = nullptr;
	}

	int32 Application::Run()
	{
		return 0;
	}

}