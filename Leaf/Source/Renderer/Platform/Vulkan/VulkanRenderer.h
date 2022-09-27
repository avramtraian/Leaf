// Copyright to Avram Traian. 2022 - 2022.
// File created on September 2 2022.

#pragma once

#include "Renderer/Renderer.h"

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

/** Build Configuration switch. */
#if LF_CONFIGURATION_DEBUG
#	define LF_VULKAN_ENABLE_VALIDATION (1)
#elif LF_CONFIGURATION_RELEASE
#	define LF_VULKAN_ENABLE_VALIDATION (1)
#elif LF_CONFIGURATION_SHIPPING
#	define LF_VULKAN_ENABLE_VALIDATION (0)
#endif

#if LF_VULKAN_ENABLE_VALIDATION
#	define LF_VK_CHECK(EXPRESSION)      \
		if ((EXPRESSION) != VK_SUCCESS) \
		{                               \
			LF_DEBUGBREAK();            \
		}

#	define LF_VK_CHECK_RETURN(EXPRESSION, RETURN_VALUE) \
		if ((EXPRESSION) != VK_SUCCESS)                 \
		{                                               \
			LF_DEBUGBREAK();                            \
			return (RETURN_VALUE);                      \
		}
#else
#	define LF_VK_CHECK(EXPRESSION)                      EXPRESSION
#	define LF_VK_CHECK_RETURN(EXPRESSION, RETURN_VALUE) EXPRESSION
#endif

namespace Leaf { namespace Renderer { namespace VulkanRenderer {

	bool Initialize();

	void Shutdown();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////// RENDERING CONTEXT /////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Result CreateRenderingContext(Ref<RenderingContext>& out_rendering_context, const RenderingContextSpecification& specification);

	class VulkanContext : public RenderingContext
	{
	public:
		VulkanContext(const RenderingContextSpecification& specification, Result& out_result);
		virtual ~VulkanContext() override;

	private:
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		VkQueue m_PresentQueue = VK_NULL_HANDLE;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////// FRAMEBUFFER ///////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Result CreateFramebuffer(Ref<Framebuffer>& out_framebuffer, const FrambufferSpecification& specification);

	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FrambufferSpecification& specification, Result& out_result);
		virtual ~VulkanFramebuffer() override;

	private:
		VkFramebuffer m_Handle = VK_NULL_HANDLE;
	};

} } }