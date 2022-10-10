// Copyright to Avram Traian. 2022 - 2022.
// File created on September 29 2022.

#pragma once

namespace Leaf { namespace Renderer {

	struct PipelineSpecification
	{

	};

	class Pipeline : public RefCounted
	{
	protected:
		Pipeline(const PipelineSpecification& specification)
			: m_Specification(specification)
		{}

	public:
		virtual ~Pipeline() = default;

	protected:
		PipelineSpecification m_Specification;
	};

} }