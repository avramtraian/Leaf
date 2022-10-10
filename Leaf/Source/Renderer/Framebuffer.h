// Copyright to Avram Traian. 2022 - 2022.
// File created on September 2 2022.

#pragma once

namespace Leaf { namespace Renderer {

	struct FrambufferSpecification
	{

	};

	class Framebuffer : public RefCounted
	{
	protected:
		Framebuffer(const FrambufferSpecification& specification)
			: m_Specification(specification)
		{}

	public:
		virtual ~Framebuffer() = default;

	public:
		const FrambufferSpecification& GetSpecification() const { return m_Specification; }

	protected:
		FrambufferSpecification m_Specification;
	};

} }