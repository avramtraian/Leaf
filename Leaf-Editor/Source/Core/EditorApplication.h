// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include <Core/Application.h>

namespace Leaf {

	class EditorApplication : public Application
	{
	public:
		EditorApplication(const ApplicationSpecification& specification);
		virtual ~EditorApplication() override;
	};

}