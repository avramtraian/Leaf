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

#	define LF_VK_CHECK_RETURN_VOID(EXPRESSION) \
		if ((EXPRESSION) != VK_SUCCESS)        \
		{                                      \
			LF_DEBUGBREAK();                   \
			return;                            \
		}

#	define LF_VK_CHECK_AND(EXPRESSION, ...) \
		if ((EXPRESSION) != VK_SUCCESS)            \
		{                                          \
			LF_DEBUGBREAK();                       \
			__VA_ARGS__                            \
		}
#else
#	define LF_VK_CHECK(EXPRESSION)                      EXPRESSION
#	define LF_VK_CHECK_RETURN(EXPRESSION, RETURN_VALUE) EXPRESSION
#	define LF_VK_CHECK_RETURN_VOID(EXPRESSION)          EXPRESSION
#	define LF_VK_CHECK_AND(EXPRESSION, ...)         EXPRESSION
#endif

namespace Leaf { namespace Renderer { namespace VulkanRenderer {

	bool Initialize();

	void Shutdown();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////// SWAPCHAIN /////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class VulkanSwapchain : public Swapchain
	{
	public:
		VulkanSwapchain(const SwapchainSpecification& specification, class VulkanContext* context, Result& out_result);
		virtual ~VulkanSwapchain() override;

	private:
		VkSwapchainKHR m_Handle = VK_NULL_HANDLE;

		class VulkanContext* m_OwningContext = nullptr;

		Vector<VkImage> m_Images;

		Vector<VkImageView> m_ImageViews;
	};

	Result CreateSwapchain(Ref<VulkanSwapchain>& out_swapchain, class VulkanContext* context, const SwapchainSpecification& specification);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////// RENDERING CONTEXT /////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class VulkanContext : public RenderingContext
	{
	public:
		VulkanContext(const RenderingContextSpecification& specification, Result& out_result);
		virtual ~VulkanContext() override;

	private:
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		uint32 m_PresentQueueFamilyIndex = UINT32_MAX;

		VkQueue m_PresentQueue = VK_NULL_HANDLE;

		Ref<VulkanSwapchain> m_Swapchain;

	private:
		friend class VulkanSwapchain;

		friend Result RenderingContextCreateSwapchain(Weak<RenderingContext>, const SwapchainSpecification&);
	};

	Result CreateRenderingContext(Ref<RenderingContext>& out_rendering_context, const RenderingContextSpecification& specification);

	Result RenderingContextCreateSwapchain(Weak<RenderingContext> rendering_context, const SwapchainSpecification& specification);

	Weak<VulkanContext> GetContext();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////// SHADER /////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const ShaderSpecification& specification, Result& out_result);
		virtual ~VulkanShader() override;

	private:
	};

	Result CreateShader(Ref<Shader>& out_shader, const ShaderSpecification& specification);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////// PIPELINE ////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineSpecification& specification, Result& out_result);
		virtual ~VulkanPipeline() override;

	private:
	};

	Result CreatePipeline(Ref<Pipeline>& out_pipeline, const PipelineSpecification& specification);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////// FRAMEBUFFER ///////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FrambufferSpecification& specification, Result& out_result);
		virtual ~VulkanFramebuffer() override;

	private:
		VkFramebuffer m_Handle = VK_NULL_HANDLE;
	};

	Result CreateFramebuffer(Ref<Framebuffer>& out_framebuffer, const FrambufferSpecification& specification);

} } }