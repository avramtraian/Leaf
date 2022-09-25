// Copyright to Avram Traian. 2022 - 2022.
// File created on September 2 2022.

#include "Renderer.h"

#include "Platform/Vulkan/VulkanRenderer.h"

namespace Leaf { namespace Renderer {

	RendererConfiguration Config;

	struct RendererCallTable
	{
		bool(*Initialize)() = nullptr;
		void(*Shutdown)() = nullptr;

		Result(*CreateRenderingContext)(Ref<RenderingContext>&, const RenderingContextSpecification&) = nullptr;
		Result(*CreateFramebuffer)(Ref<Framebuffer>&, const FrambufferSpecification&) = nullptr;
	};

	struct RendererData
	{
		RendererCallTable CallTable;
	};
	static RendererData* s_RendererData = nullptr;

	bool Initialize()
	{
		if (s_RendererData)
			return false;

		s_RendererData = lnew RendererData();
		if (!s_RendererData)
			return false;

		switch (Config.RenderingAPI)
		{
			case API::Vulkan:
			{
				s_RendererData->CallTable.Initialize = VulkanRenderer::Initialize;
				s_RendererData->CallTable.Shutdown = VulkanRenderer::Shutdown;
				s_RendererData->CallTable.CreateFramebuffer = VulkanRenderer::CreateFramebuffer;
				s_RendererData->CallTable.CreateRenderingContext = VulkanRenderer::CreateRenderingContext;
				break;
			}
		}

		return s_RendererData->CallTable.Initialize();
	}

	void Shutdown()
	{
		if (!s_RendererData)
			return;

		s_RendererData->CallTable.Shutdown();

		ldelete s_RendererData;
		s_RendererData = nullptr;
	}

	Result CreateRenderingContext(Ref<RenderingContext>& out_rendering_context, const RenderingContextSpecification& specification)
	{
		return s_RendererData->CallTable.CreateRenderingContext(out_rendering_context, specification);
	}

	Result CreateFramebuffer(Ref<Framebuffer>& out_framebuffer, const FrambufferSpecification& specification)
	{
		return s_RendererData->CallTable.CreateFramebuffer(out_framebuffer, specification);
	}

} }