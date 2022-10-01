// Copyright to Avram Traian. 2022 - 2022.
// File created on September 2 2022.

#include "VulkanRenderer.h"

#include "Core/Platform/Platform.h"
#include "Core/Math/Math.h"

#include <shaderc/shaderc.hpp>

/** Platform switch. */
#if LF_PLATFORM_WINDOWS
#	include "Core/Platform/Windows/WindowsHeaders.h"
#	include <vulkan/vulkan_win32.h>
#elif LF_PLATFORM_LINUX
#elif LF_PLATFORM_MACOS
#endif

namespace Leaf { namespace Renderer { namespace VulkanRenderer {

	struct PhysicalDevice
	{
		VkPhysicalDevice Handle = VK_NULL_HANDLE;

		struct
		{
			bool HasGraphicsQueueFamily = false;
			bool IsGraphicsQueueFamilyIndexShared = true;
			uint32 GraphicsQueueFamilyIndex = UINT32_MAX;

			bool HasTransferQueueFamily = false;
			bool IsTransferQueueFamilyIndexShared = true;
			uint32 TransferQueueFamilyIndex = UINT32_MAX;

			bool HasComputeQueueFamily = false;
			bool IsComputeQueueFamilyIndexShared = true;
			uint32 ComputeQueueFamilyIndex = UINT32_MAX;
		}
		QueueFamilies;
	};

	struct VulkanRendererData
	{
		VkInstance Instance = VK_NULL_HANDLE;

		PhysicalDevice PhysicalDevice;

		VkDevice LogicalDevice = VK_NULL_HANDLE;

		VkQueue GraphicsQueue = VK_NULL_HANDLE;
		VkQueue TransferQueue = VK_NULL_HANDLE;
		VkQueue ComputeQueue = VK_NULL_HANDLE;

		Vector<uint32> CreatedQueueFamilyIndices;

#if LF_VULKAN_ENABLE_VALIDATION
		VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
#endif
	};
	static VulkanRendererData* s_VulkanData = nullptr;

	namespace Utils {

		static VkBool32 DebuggerCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void*                                       pUserData)
		{
			switch (messageSeverity)
			{
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
					// LF_RENDERER_TRACE("%{}", (const char*)pCallbackData->pMessage);
					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
					// LF_RENDERER_INFO("%{}", (const char*)pCallbackData->pMessage);
					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
					LF_RENDERER_WARN("%{}", (const char*)pCallbackData->pMessage);
					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
					LF_RENDERER_ERROR("%{}", (const char*)pCallbackData->pMessage);
					break;
			}

			return VK_FALSE;
		}

		static bool PhysicalDeviceHasAllExtensions(const PhysicalDevice& device, const Vector<const char*>& required_extensions)
		{
			uint32 device_extensions_count;
			vkEnumerateDeviceExtensionProperties(device.Handle, nullptr, &device_extensions_count, nullptr);

			Vector<VkExtensionProperties> device_extensions;
			device_extensions.SetSizeUninitialized(device_extensions_count);
			vkEnumerateDeviceExtensionProperties(device.Handle, nullptr, &device_extensions_count, device_extensions.Data());

			bool has_all_extensions = true;
			for (auto& required_extension : required_extensions)
			{
				bool was_found = false;

				for (auto& available_extension : device_extensions)
				{
					if (StringCalls::Equals(required_extension, available_extension.extensionName))
					{
						was_found = true;
						break;
					}
				}

				if (!was_found)
					return false;
			}

			return true;
		}

		static void PhysicalDeviceFindQueueFamilyIndices(PhysicalDevice& device)
		{
			uint32 queue_families_count;
			vkGetPhysicalDeviceQueueFamilyProperties(device.Handle, &queue_families_count, nullptr);

			Vector<VkQueueFamilyProperties> queue_families;
			queue_families.SetSizeUninitialized((SizeT)queue_families_count);
			vkGetPhysicalDeviceQueueFamilyProperties(device.Handle, &queue_families_count, queue_families.Data());

			Vector<int32> queue_graphics_scores(queue_families.Size());
			Vector<int32> queue_transfer_scores(queue_families.Size());
			Vector<int32> queue_compute_scores(queue_families.Size());

			for (uint32 index = 0; index < queue_families.Size(); index++)
			{
				auto& queue_family = queue_families[index];
				queue_graphics_scores.Add(0);
				queue_transfer_scores.Add(0);
				queue_compute_scores.Add(0);

				if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					queue_graphics_scores[index] += 4;
					queue_transfer_scores[index] -= 1;
					queue_compute_scores[index] -= 1;
				}
				if (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT)
				{
					queue_graphics_scores[index] -= 1;
					queue_transfer_scores[index] += 4;
					queue_compute_scores[index] -= 1;
				}
				if (queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT)
				{
					queue_graphics_scores[index] -= 1;
					queue_transfer_scores[index] -= 1;
					queue_compute_scores[index] += 4;
				}
			}

			int32 best_queue_graphics_score = queue_graphics_scores[0];
			int32 best_queue_transfer_score = queue_transfer_scores[0];
			int32 best_queue_compute_score = queue_compute_scores[0];

			for (uint32 index = 0; index < queue_families.Size(); index++)
			{
				if (queue_graphics_scores[index] >= best_queue_graphics_score)
				{
					best_queue_graphics_score = queue_graphics_scores[index];
					if (best_queue_graphics_score > 0)
					{
						device.QueueFamilies.HasGraphicsQueueFamily = true;
						device.QueueFamilies.GraphicsQueueFamilyIndex = index;

						if (!(queue_families[index].queueFlags & VK_QUEUE_TRANSFER_BIT) && !(queue_families[index].queueFlags & VK_QUEUE_COMPUTE_BIT))
							device.QueueFamilies.IsGraphicsQueueFamilyIndexShared = false;
					}
				}
				if (queue_transfer_scores[index] >= best_queue_transfer_score)
				{
					best_queue_transfer_score = queue_transfer_scores[index];
					if (best_queue_transfer_score > 0)
					{
						device.QueueFamilies.HasTransferQueueFamily = true;
						device.QueueFamilies.TransferQueueFamilyIndex = index;

						if (!(queue_families[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !(queue_families[index].queueFlags & VK_QUEUE_COMPUTE_BIT))
							device.QueueFamilies.IsGraphicsQueueFamilyIndexShared = false;
					}
				}
				if (queue_compute_scores[index] >= best_queue_compute_score)
				{
					best_queue_compute_score = queue_compute_scores[index];
					if (best_queue_compute_score > 0)
					{
						device.QueueFamilies.HasComputeQueueFamily = true;
						device.QueueFamilies.ComputeQueueFamilyIndex = index;

						if (!(queue_families[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !(queue_families[index].queueFlags & VK_QUEUE_TRANSFER_BIT))
							device.QueueFamilies.IsGraphicsQueueFamilyIndexShared = false;
					}
				}
			}
		}

		static PhysicalDevice PhysicalDevicePickBest(const Vector<VkPhysicalDevice>& devices, const Vector<const char*>& required_device_extensions)
		{
			uint32 best_score = 0;
			PhysicalDevice picked_device;

			for (auto& device_handle : devices)
			{
				uint32 device_score = 0;
				PhysicalDevice device;
				device.Handle = device_handle;

				// Device is not suitable, no matter the score.
				if (!PhysicalDeviceHasAllExtensions(device, required_device_extensions))
					continue;

				VkPhysicalDeviceProperties device_properties;
				vkGetPhysicalDeviceProperties(device_handle, &device_properties);

				VkPhysicalDeviceFeatures device_features;
				vkGetPhysicalDeviceFeatures(device_handle, &device_features);

				// TODO (Avr): Read these values from a config file.
				constexpr uint32 DeviceScoreWeight_IsDiscrete       = 3;
				constexpr uint32 DeviceScoreWeight_HasGraphicsQueue = 4;
				constexpr uint32 DeviceScoreWeight_HasTransferQueue = 1;
				constexpr uint32 DeviceScoreWeight_HasComputeQueue  = 1;

				if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
					device_score += DeviceScoreWeight_IsDiscrete;

				PhysicalDeviceFindQueueFamilyIndices(device);

				// Device is not suitable, no matter the score.
				if (!device.QueueFamilies.HasGraphicsQueueFamily)
					continue;

				if (device.QueueFamilies.HasGraphicsQueueFamily)
					device_score += DeviceScoreWeight_HasGraphicsQueue;

				if (device.QueueFamilies.HasTransferQueueFamily)
					device_score += DeviceScoreWeight_HasTransferQueue;

				if (device.QueueFamilies.HasComputeQueueFamily)
					device_score += DeviceScoreWeight_HasComputeQueue;

				if (device_score >= best_score)
				{
					best_score = device_score;
					picked_device = device;
				}
			}

			return picked_device;
		}

	}

	bool Initialize()
	{
		if (s_VulkanData)
			return false;

		LF_RENDERER_INFO("Initializing the Renderer...");

		s_VulkanData = lnew VulkanRendererData();

		LF_RENDERER_TRACE("Creating the Vulkan Instance...");

		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.apiVersion = VK_API_VERSION_1_3;
		app_info.pApplicationName = "Leaf";
		app_info.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
		app_info.pEngineName = "Leaf";
		app_info.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);

		Vector<const char*> extension_names;

		extension_names.Add(VK_KHR_SURFACE_EXTENSION_NAME);

/** Platform switch. */
#if LF_PLATFORM_WINDOWS
		extension_names.Add("VK_KHR_win32_surface");
#elif LF_PLATFORM_MACOS
#elif LF_PLATFORM_LINUX
#endif

#if LF_VULKAN_ENABLE_VALIDATION
		extension_names.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		if (!extension_names.IsEmpty())
		{
			LF_RENDERER_TRACE("Required Vulkan extensions:");
			for (auto& extension : extension_names)
				LF_RENDERER_TRACE("    - %{}", extension);
		}

		Vector<const char*> required_validation_layers;

#if LF_VULKAN_ENABLE_VALIDATION
		LF_RENDERER_INFO("Vulkan validation layers are enabled.");

		required_validation_layers.Add("VK_LAYER_KHRONOS_validation");

		// Gets the available layers.
		uint32 available_layers_count = 0;
		LF_VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layers_count, nullptr));
		Vector<VkLayerProperties> available_layers;
		available_layers.SetSizeUninitialized(available_layers_count);
		LF_VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layers_count, available_layers.Data()));

		// Ensures that all the required validation layers are available.
		for (auto& required_layer : required_validation_layers)
		{
			LF_RENDERER_TRACE("Checking that layer '%{}' is available...", required_layer);

			bool was_found = false;
			for (auto& available_layer : available_layers)
			{
				if (StringCalls::Equals(required_layer, available_layer.layerName))
				{
					was_found = true;
					break;
				}
			}

			if (!was_found)
			{
				LF_RENDERER_FATAL("Required validation layer '%{}' is missing!", required_layer);
				return false;
			}
		}

		LF_RENDERER_INFO("All required validation layers are available.");

#else
		LF_RENDERER_INFO("Vulkan validation layers are disabled.");
#endif

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;
		create_info.ppEnabledExtensionNames = extension_names.Data();
		create_info.enabledExtensionCount = (uint32)extension_names.Size();
		create_info.ppEnabledLayerNames = required_validation_layers.Data();
		create_info.enabledLayerCount = (uint32)required_validation_layers.Size();

		// Vulkan Instance creation failed.
		LF_VK_CHECK_RETURN(vkCreateInstance(&create_info, nullptr, &s_VulkanData->Instance), false);

		LF_RENDERER_INFO("Vulkan Instance created successfully.");

#if LF_VULKAN_ENABLE_VALIDATION
		LF_RENDERER_TRACE("Creating the Vulkan Debug Messenger...");

		VkDebugUtilsMessengerCreateInfoEXT debugger_create_info = {};
		debugger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

		debugger_create_info.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT   |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

		debugger_create_info.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

		debugger_create_info.pfnUserCallback = Utils::DebuggerCallback;

		PFN_vkCreateDebugUtilsMessengerEXT debugger_create = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_VulkanData->Instance, "vkCreateDebugUtilsMessengerEXT");
		LF_ASSERT_RETURN(debugger_create != nullptr, false);

		LF_VK_CHECK(debugger_create(s_VulkanData->Instance, &debugger_create_info, nullptr, &s_VulkanData->DebugMessenger));
		LF_RENDERER_INFO("Vulkan Debugger Messenger created successfully.");
#endif

		uint32 devices_count;
		LF_VK_CHECK(vkEnumeratePhysicalDevices(s_VulkanData->Instance, &devices_count, nullptr));

		Vector<VkPhysicalDevice> devices;
		devices.SetSizeUninitialized(devices_count);
		LF_VK_CHECK(vkEnumeratePhysicalDevices(s_VulkanData->Instance, &devices_count, devices.Data()));

		// TODO (Avr): Macros.
		if (devices.IsEmpty())
		{
			LF_RENDERER_FATAL("Failed to find a GPU with Vulkan support! Aborting.");
			return false;
		}

		LF_RENDERER_INFO("Available GPUs:");
		for (auto& device : devices)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);
			LF_RENDERER_INFO("    - %{}", (const char*)properties.deviceName);
		}

		Vector<const char*> device_extensions;
		device_extensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		s_VulkanData->PhysicalDevice = Utils::PhysicalDevicePickBest(devices, device_extensions);

		// TODO (Avr): Macros.
		if (s_VulkanData->PhysicalDevice.Handle == VK_NULL_HANDLE)
		{
			LF_RENDERER_FATAL("Failed to find a GPU with all the required Vulkan features! Aborting.");
			return false;
		}

		VkPhysicalDeviceProperties gpu_properties = {};
		vkGetPhysicalDeviceProperties(s_VulkanData->PhysicalDevice.Handle, &gpu_properties);
		LF_RENDERER_INFO("Renderer Hardware: %{}", (const char*)(&gpu_properties.deviceName[0]));

		LF_RENDERER_TRACE("Creating the Vulkan Logical Device...");

		Vector<VkDeviceQueueCreateInfo> queue_create_infos = {};
		Array<uint32, 3> required_queue_family_indices = {
			s_VulkanData->PhysicalDevice.QueueFamilies.GraphicsQueueFamilyIndex,
			s_VulkanData->PhysicalDevice.QueueFamilies.TransferQueueFamilyIndex,
			s_VulkanData->PhysicalDevice.QueueFamilies.ComputeQueueFamilyIndex
		};

		for (SizeT index = 0; index < required_queue_family_indices.Size(); index++)
		{
			bool created = false;
			for (auto& already_created_index : s_VulkanData->CreatedQueueFamilyIndices)
			{
				if (required_queue_family_indices[index] == already_created_index)
				{
					created = true;
					break;
				}
			}
			if (created)
				continue;

			VkDeviceQueueCreateInfo queue_create_info = {};
			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = required_queue_family_indices[index];
			queue_create_info.queueCount = 1;
			float queue_priority = 1.0f;
			queue_create_info.pQueuePriorities = &queue_priority;

			queue_create_infos.Add(queue_create_info);
			s_VulkanData->CreatedQueueFamilyIndices.Add(required_queue_family_indices[index]);
		}

		VkDeviceCreateInfo device_create_info = {};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		VkPhysicalDeviceFeatures features = {};
		device_create_info.pEnabledFeatures = &features;

		device_create_info.queueCreateInfoCount = (uint32)queue_create_infos.Size();
		device_create_info.pQueueCreateInfos = queue_create_infos.Data();

		device_create_info.enabledExtensionCount = (uint32)device_extensions.Size();
		device_create_info.ppEnabledExtensionNames = device_extensions.Data();

		// Deprecated and ignored, so pass nothing.
		device_create_info.enabledLayerCount = 0;
		device_create_info.ppEnabledLayerNames = nullptr;

		LF_VK_CHECK_RETURN(vkCreateDevice(s_VulkanData->PhysicalDevice.Handle, &device_create_info, nullptr, &s_VulkanData->LogicalDevice), false);
		LF_RENDERER_INFO("Vulkan Logical Device created successfully.");

		LF_RENDERER_TRACE("Aquiring the device queue handles...");
		vkGetDeviceQueue(s_VulkanData->LogicalDevice, s_VulkanData->PhysicalDevice.QueueFamilies.GraphicsQueueFamilyIndex, 0, &s_VulkanData->GraphicsQueue);
		vkGetDeviceQueue(s_VulkanData->LogicalDevice, s_VulkanData->PhysicalDevice.QueueFamilies.TransferQueueFamilyIndex, 0, &s_VulkanData->TransferQueue);
		vkGetDeviceQueue(s_VulkanData->LogicalDevice, s_VulkanData->PhysicalDevice.QueueFamilies.ComputeQueueFamilyIndex, 0, &s_VulkanData->ComputeQueue);

		LF_RENDERER_INFO("Renderer initialized successfully.");
		return true;
	}

	void Shutdown()
	{
		if (!s_VulkanData)
			return;

		LF_RENDERER_INFO("Shutting down the Renderer...");

		s_VulkanData->GraphicsQueue = VK_NULL_HANDLE;
		s_VulkanData->TransferQueue = VK_NULL_HANDLE;
		s_VulkanData->ComputeQueue = VK_NULL_HANDLE;

		vkDestroyDevice(s_VulkanData->LogicalDevice, nullptr);
		s_VulkanData->LogicalDevice = VK_NULL_HANDLE;
		LF_RENDERER_INFO("Vulkan Logical Device was destroyed.");

#if LF_VULKAN_ENABLE_VALIDATION
		PFN_vkDestroyDebugUtilsMessengerEXT debugger_destroy = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_VulkanData->Instance, "vkDestroyDebugUtilsMessengerEXT");
		LF_ASSERT(debugger_destroy);

		debugger_destroy(s_VulkanData->Instance, s_VulkanData->DebugMessenger, nullptr);
		s_VulkanData->DebugMessenger = VK_NULL_HANDLE;
		LF_RENDERER_INFO("Vulkan Debugger Messenger was destroyed.");
#endif

		vkDestroyInstance(s_VulkanData->Instance, nullptr);
		s_VulkanData->Instance = VK_NULL_HANDLE;
		LF_RENDERER_INFO("Vulkan Instance was destroyed.");

		ldelete s_VulkanData;
		s_VulkanData = nullptr;

		LF_RENDERER_INFO("Renderer was shut down.");
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////// SWAPCHAIN /////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	namespace Utils {

		struct SwapchainSupport
		{
			VkSurfaceCapabilitiesKHR Capabilities = {};
			Vector<VkSurfaceFormatKHR> Formats;
			Vector<VkPresentModeKHR> PresentModes;
		};

		static void SwapchainQueryPhysicalDeviceForSupport(VkSurfaceKHR surface, SwapchainSupport& out_support, Result& out_result)
		{
			LF_VK_CHECK_AND(
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(s_VulkanData->PhysicalDevice.Handle, surface, &out_support.Capabilities),

				LF_RENDERER_ERROR("Failed to aquire the VkPhysicalDeviceSurfaceCapabilities!");
			out_result = Result::Failure;
			return;
			);

			uint32 formats_count;
			vkGetPhysicalDeviceSurfaceFormatsKHR(s_VulkanData->PhysicalDevice.Handle, surface, &formats_count, nullptr);
			out_support.Formats.SetSizeUninitialized(formats_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(s_VulkanData->PhysicalDevice.Handle, surface, &formats_count, out_support.Formats.Data());

			uint32 present_modes_count;
			vkGetPhysicalDeviceSurfacePresentModesKHR(s_VulkanData->PhysicalDevice.Handle, surface, &present_modes_count, nullptr);
			out_support.PresentModes.SetSizeUninitialized(present_modes_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(s_VulkanData->PhysicalDevice.Handle, surface, &present_modes_count, out_support.PresentModes.Data());
		}

	}

	VulkanSwapchain::VulkanSwapchain(const SwapchainSpecification& specification, class VulkanContext* context, Result& out_result)
		: Swapchain(specification)
		, m_OwningContext(context)
	{
		out_result = Result::Success;

		LF_RENDERER_TRACE("Creating the Vulkan Swapchain...");

		LF_RENDERER_TRACE("Querying the physical device for Vulkan Surface Support...");
		Utils::SwapchainSupport support;
		Utils::SwapchainQueryPhysicalDeviceForSupport(m_OwningContext->m_Surface, support, out_result);

		if (out_result != Result::Success)
		{
			LF_RENDERER_ERROR("Failed to query the swapchain support!");
			return;
		}
		if (support.Formats.IsEmpty())
		{
			out_result = Result::Failure;
			LF_RENDERER_ERROR("The selected device has no supported swapchain formats!");
			return;
		}
		if (support.PresentModes.IsEmpty())
		{
			out_result = Result::Failure;
			LF_RENDERER_ERROR("The selected device has no supported swapchain present modes!");
			return;
		}

		// NOTE (Avr): If the ideal format is not available, the first format is usually good enough.
		VkSurfaceFormatKHR surface_format = support.Formats[0];
		for (auto& format : support.Formats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				surface_format = format;
		}

		// NOTE (Avr): Guaranteed to be available.
		VkPresentModeKHR present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
		for (auto& pm : support.PresentModes)
		{
			if (pm == VK_PRESENT_MODE_FIFO_KHR)
				present_mode = pm;
		}

		VkExtent2D extent = support.Capabilities.currentExtent;
		if (support.Capabilities.currentExtent.width == UINT32_MAX)
		{
			extent.width  = Math::Clamp(m_OwningContext->m_Specification.Window->GetWidth(),  support.Capabilities.minImageExtent.width,  support.Capabilities.maxImageExtent.width);
			extent.height = Math::Clamp(m_OwningContext->m_Specification.Window->GetHeight(), support.Capabilities.minImageExtent.height, support.Capabilities.maxImageExtent.height);
		}

		uint32 image_count = support.Capabilities.minImageCount + 1;
		if (support.Capabilities.maxImageCount > 0 && image_count > support.Capabilities.maxImageCount)
			image_count = support.Capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR sc_create_info = {};
		sc_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

		sc_create_info.minImageCount = image_count;
		sc_create_info.imageFormat = surface_format.format;
		sc_create_info.imageColorSpace = surface_format.colorSpace;
		sc_create_info.imageExtent = extent;
		sc_create_info.imageArrayLayers = 1;
		sc_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		if (s_VulkanData->PhysicalDevice.QueueFamilies.GraphicsQueueFamilyIndex != m_OwningContext->m_PresentQueueFamilyIndex)
		{
			uint32 queue_family_indices[2] =
			{
				s_VulkanData->PhysicalDevice.QueueFamilies.GraphicsQueueFamilyIndex,
				m_OwningContext->m_PresentQueueFamilyIndex
			};

			sc_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			sc_create_info.queueFamilyIndexCount = sizeof(queue_family_indices) / sizeof(queue_family_indices[0]);
			sc_create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else
		{
			sc_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			sc_create_info.queueFamilyIndexCount = 0;
			sc_create_info.pQueueFamilyIndices = nullptr;
		}

		sc_create_info.preTransform = support.Capabilities.currentTransform;
		sc_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		sc_create_info.presentMode = present_mode;
		sc_create_info.clipped = VK_TRUE;

		sc_create_info.surface = m_OwningContext->m_Surface;
		sc_create_info.oldSwapchain = VK_NULL_HANDLE;

		LF_VK_CHECK_AND(
			vkCreateSwapchainKHR(s_VulkanData->LogicalDevice, &sc_create_info, nullptr, &m_Handle),

			LF_RENDERER_ERROR("Failed to create the Vulkan Swapchain!");
			out_result = Result::Failure;
			return;
		);

		LF_RENDERER_INFO("Vulkan Swapchain created successfully.");

		LF_RENDERER_TRACE("VulkanSwapchain::VulkanSwapchain: Creating the views for the swapchain's images.");
		uint32 swapchain_images_count;
		vkGetSwapchainImagesKHR(s_VulkanData->LogicalDevice, m_Handle, &swapchain_images_count, nullptr);
		m_Images.SetSizeUninitialized(swapchain_images_count);
		vkGetSwapchainImagesKHR(s_VulkanData->LogicalDevice, m_Handle, &swapchain_images_count, m_Images.Data());

		m_ImageViews.SetSize(m_Images.Size());
		for (SizeT index = 0; index < m_Images.Size(); index++)
		{
			VkImageViewCreateInfo iv_create_info = {};
			iv_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

			iv_create_info.image = m_Images[index];
			iv_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			iv_create_info.format = surface_format.format;

			iv_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			iv_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			iv_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			iv_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			iv_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			iv_create_info.subresourceRange.levelCount = 1;
			iv_create_info.subresourceRange.baseMipLevel = 0;
			iv_create_info.subresourceRange.layerCount = 1;
			iv_create_info.subresourceRange.baseArrayLayer = 0;

			LF_VK_CHECK_AND(
				vkCreateImageView(s_VulkanData->LogicalDevice, &iv_create_info, nullptr, &m_ImageViews[index]),

				LF_RENDERER_ERROR("VulkanSwapchain::VulkanSwapchain: Failed to create an image view!");
				out_result = Result::Failure;
				return;
			);
		}

		LF_RENDERER_INFO("VulkanSwapchain::VulkanSwapchain: Swapchain's images' views created.");
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		LF_RENDERER_TRACE("Destroying the Vulkan Swapchain...");

		for (auto& image_view : m_ImageViews)
			vkDestroyImageView(s_VulkanData->LogicalDevice, image_view, nullptr);

		vkDestroySwapchainKHR(s_VulkanData->LogicalDevice, m_Handle, nullptr);

		LF_RENDERER_INFO("The Vulkan Swapchain was destroyed.");
	}

	Result CreateSwapchain(Ref<VulkanSwapchain>& out_swapchain, VulkanContext* context, const SwapchainSpecification& specification)
	{
		Result result;
		Ref<VulkanSwapchain> swapchain = Ref<VulkanSwapchain>::Create(specification, context, result);

		out_swapchain = (result == Result::Success) ? swapchain : nullptr;
		return result;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////// RENDERING CONTEXT /////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VulkanContext::VulkanContext(const RenderingContextSpecification& specification, Result& out_result)
		: RenderingContext(specification)
	{
		out_result = Result::Success;

		LF_RENDERER_INFO("Creating a Vulkan Rendering Context...");
		LF_RENDERER_TRACE("    |_ Owning window is '%{}' (NativeHandle: %{}).", m_Specification.Window->GetTitle().ToView(), m_Specification.Window->GetNativeHandle());

		/** Platform switch. */
#if LF_PLATFORM_WINDOWS
		VkWin32SurfaceCreateInfoKHR surface_create_info = {};
		surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surface_create_info.hwnd = (HWND)m_Specification.Window->GetNativeHandle();
		surface_create_info.hinstance = (HINSTANCE)Platform::GetProcessHandle();

		VkResult surface_result = vkCreateWin32SurfaceKHR(s_VulkanData->Instance, &surface_create_info, nullptr, &m_Surface);
#elif LF_PLATFORM_LINUX
#elif LF_PLATFORM_MACOS
#endif

		if (surface_result != VK_SUCCESS)
		{
			out_result = Result::Failure;
			LF_RENDERER_FATAL("Failed to create the window surface!");
			return;
		}

		LF_RENDERER_TRACE("Searching for the present device queue family index...");
		bool has_present_queue = false;

		// NOTE (Avr): Try to have drawing and presentation in the the same queue for better performance.
		VkBool32 supported = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(s_VulkanData->PhysicalDevice.Handle, s_VulkanData->PhysicalDevice.QueueFamilies.GraphicsQueueFamilyIndex, m_Surface, &supported);
		if (supported)
		{
			has_present_queue = true;
			m_PresentQueueFamilyIndex = s_VulkanData->PhysicalDevice.QueueFamilies.GraphicsQueueFamilyIndex;
		}

		if (!has_present_queue)
		{
			uint32 queue_families_count;
			vkGetPhysicalDeviceQueueFamilyProperties(s_VulkanData->PhysicalDevice.Handle, &queue_families_count, nullptr);

			for (auto& queue_family_index : s_VulkanData->CreatedQueueFamilyIndices)
			{
				VkBool32 supported = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(s_VulkanData->PhysicalDevice.Handle, queue_family_index, m_Surface, &supported);
				if (supported)
				{
					has_present_queue = true;
					m_PresentQueueFamilyIndex = s_VulkanData->PhysicalDevice.QueueFamilies.GraphicsQueueFamilyIndex;
					break;
				}
			}

			if (!has_present_queue)
			{
				out_result = Result::Failure;
				LF_RENDERER_FATAL("No device queue family has surface support!");
				return;
			}
		}

		vkGetDeviceQueue(s_VulkanData->LogicalDevice, m_PresentQueueFamilyIndex, 0, &m_PresentQueue);
		LF_RENDERER_INFO("The Vulkan Present Queue was aquired.");
	}

	VulkanContext::~VulkanContext()
	{
		LF_RENDERER_TRACE("Destroying a Vulkan Rendering Context...");
		LF_RENDERER_TRACE("    |_ Owning window is '%{}' (NativeHandle: %{}).", m_Specification.Window->GetTitle().ToView(), m_Specification.Window->GetNativeHandle());

		m_Swapchain.Release();

		vkDestroySurfaceKHR(s_VulkanData->Instance, m_Surface, nullptr);

		LF_RENDERER_INFO("A Vulkan Rendering Context was destroyed.");
		LF_RENDERER_TRACE("    |_ Owning window is '%{}' (NativeHandle: %{}).", m_Specification.Window->GetTitle().ToView(), m_Specification.Window->GetNativeHandle());
	}

	Result CreateRenderingContext(Ref<RenderingContext>& out_rendering_context, const RenderingContextSpecification& specification)
	{
		Result result;
		Ref<VulkanContext> context = Ref<VulkanContext>::Create(specification, result);

		out_rendering_context = (result == Result::Success) ? context.As<RenderingContext>() : nullptr;
		return result;
	}

	Result RenderingContextCreateSwapchain(Weak<RenderingContext> rendering_context, const SwapchainSpecification& specification)
	{
		VulkanContext* context = rendering_context.As<VulkanContext>().Get();

		Result result = CreateSwapchain(context->m_Swapchain, context, specification);
		LF_ASSERT_AND(
			result == Result::Success,

			LF_RENDERER_ERROR("RenderingContextCreateSwapchain: Failed to create the Vulkan Swapchain!");
			return result;
		);

		return Result::Success;
	}

	Weak<VulkanContext> GetContext()
	{
		return Renderer::GetActiveRenderingContext().As<VulkanContext>();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////// SHADER /////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	namespace Utils {



	}

	VulkanShader::VulkanShader(const ShaderSpecification& specification, Result& out_result)
		: Shader(specification)
	{
		out_result = Result::Success;
	}

	VulkanShader::~VulkanShader()
	{
	}

	Result CreateShader(Ref<Shader>& out_shader, const ShaderSpecification& specification)
	{
		Result result;
		Ref<VulkanShader> shader = Ref<VulkanShader>::Create(specification, result);

		out_shader = (result == Result::Success) ? shader.As<Shader>() : nullptr;
		return result;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////// PIPELINE ////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VulkanPipeline::VulkanPipeline(const PipelineSpecification& specification, Result& out_result)
		: Pipeline(specification)
	{
		out_result = Result::Success;
	}

	VulkanPipeline::~VulkanPipeline()
	{
	}

	Result CreatePipeline(Ref<Pipeline>& out_pipeline, const PipelineSpecification& specification)
	{
		Result result;
		Ref<VulkanPipeline> pipeline = Ref<VulkanPipeline>::Create(specification, result);

		out_pipeline = (result == Result::Success) ? pipeline.As<Pipeline>() : nullptr;
		return result;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////// FRAMEBUFFER ///////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VulkanFramebuffer::VulkanFramebuffer(const FrambufferSpecification& specification, Result& out_result)
		: Framebuffer(specification)
	{
		out_result = Result::Success;
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	Result CreateFramebuffer(Ref<Framebuffer>& out_framebuffer, const FrambufferSpecification& specification)
	{
		Result result;
		Ref<VulkanFramebuffer> framebuffer = Ref<VulkanFramebuffer>::Create(specification, result);

		out_framebuffer = (result == Result::Success) ? framebuffer.As<Framebuffer>() : nullptr;
		return result;
	}

} } }