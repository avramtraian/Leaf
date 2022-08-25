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