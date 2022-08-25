// Copyright to Avram Traian. 2022 - 2022.
// File created on August 21 2022.

#pragma once

#include "CoreTypes.h"

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
		String Name;

		CommandLineArguments CommandArguments;
	};

	class LEAF_API Application
	{
	public:
		static Application* Get() { return s_Instance; }

	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

	public:
		int32 Run();

	private:
		ApplicationSpecification m_Specification;

	private:
		static Application* s_Instance;
	};

}