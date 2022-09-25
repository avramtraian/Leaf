// Copyright to Avram Traian. 2022 - 2022.
// File created on September 2 2022.

#pragma once

#include "Engine/Window.h"

namespace Leaf { namespace Renderer {

	struct RenderingContextSpecification
	{
		Window* Window = nullptr;
	};

	class RenderingContext : public RefCounted
	{
	public:
		RenderingContext(const RenderingContextSpecification& specification)
			: m_Specification(specification)
		{}
		virtual ~RenderingContext() = default;

	public:
		const RenderingContextSpecification& GetSpecification() const { return m_Specification; }

	private:
		RenderingContextSpecification m_Specification;
	};

} }