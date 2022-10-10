// Copyright to Avram Traian. 2022 - 2022.
// File created on September 27 2022.

#pragma once

#include "RenderingContext.h"

namespace Leaf { namespace Renderer {

	struct SwapchainSpecification
	{
	};

	class Swapchain : public RefCounted
	{
	protected:
		Swapchain(const SwapchainSpecification& specification)
			: m_Specification(specification)
		{}

	public:
		virtual ~Swapchain() = default;

	protected:
		SwapchainSpecification m_Specification;
	};

} }