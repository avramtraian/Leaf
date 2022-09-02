// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#include "EditorApplication.h"

#include <Core/Launch.h>
#include <Core/Memory.h>

namespace Leaf {

	Application* CreateApplication(const CommandLineArguments& cmd_line_args)
	{
		ApplicationSpecification specification;
		specification.Name = "Leaf-Editor";
		specification.CommandArguments = cmd_line_args;
		specification.PrimaryWindowSpecification.Title = String::Format(
			"Leaf-Editor -- %{/Platform} -- 64-bit -- %{/Configuration}",
			              LF_PLATFORM_NAME,          LF_CONFIGURATION_NAME
		);
		specification.PrimaryWindowSpecification.Width = 1280;
		specification.PrimaryWindowSpecification.Height = 720;
		specification.PrimaryWindowSpecification.PositionX = 300;
		specification.PrimaryWindowSpecification.PositionY = 300;
		specification.PrimaryWindowSpecification.Flags = WINDOW_FLAG_Primary | WINDOW_FLAG_StartMaximized;
		specification.PrimaryWindowSpecification.EventCallback = Application::OnEvent;

		return lnew EditorApplication(specification);
	}
	
	EditorApplication::EditorApplication(const ApplicationSpecification& specification)
		: Application(specification)
	{
	}

	EditorApplication::~EditorApplication()
	{
	}

}