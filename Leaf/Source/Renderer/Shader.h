// Copyright to Avram Traian. 2022 - 2022.
// File created on September 29 2022.

#pragma once

namespace Leaf { namespace Renderer {

	struct ShaderSpecification
	{
		
	};

	class Shader : public RefCounted
	{
	protected:
		Shader(const ShaderSpecification& specification)
			: m_Specification(specification)
		{}

	public:
		virtual ~Shader() = default;

	protected:
		ShaderSpecification m_Specification;
	};

} }