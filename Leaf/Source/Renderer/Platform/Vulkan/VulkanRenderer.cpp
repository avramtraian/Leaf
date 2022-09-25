// Copyright to Avram Traian. 2022 - 2022.
// File created on September 2 2022.

#include "VulkanRenderer.h"

namespace Leaf { namespace Renderer { namespace VulkanRenderer {

	struct PhysicalDevice
	{
		VkPhysicalDevice PhysicalDeviceHandle = VK_NULL_HANDLE;

		bool HasGraphicsQueue = false;
		bool IsGraphicsQueueShared = true;
		uint32 GraphicsQueueIndex = UINT32_MAX;

		bool HasTransferQueue = false;
		bool IsTransferQueueShared = true;
		uint32 TransferQueueIndex = UINT32_MAX;

		bool HasComputeQueue = false;
		bool IsComputeQueueShared = true;
		uint32 ComputeQueueIndex = UINT32_MAX;
	};

	struct VulkanRendererData
	{
		VkInstance Instance = VK_NULL_HANDLE;

		PhysicalDevice PhysicalDevice;

		VkDevice LogicalDevice = VK_NULL_HANDLE;

		VkQueue GraphicsQueue = VK_NULL_HANDLE;
		VkQueue TransferQueue = VK_NULL_HANDLE;
		VkQueue ComputeQueue = VK_NULL_HANDLE;

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

		static PhysicalDevice PickBestPhysicalDevice(const Vector<VkPhysicalDevice>& devices)
		{
			uint32 best_score = 0;
			PhysicalDevice picked_device;

			for (auto& device_handle : devices)
			{
				uint32 device_score = 0;
				PhysicalDevice device;
				device.PhysicalDeviceHandle = device_handle;

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

				uint32 queue_families_count;
				vkGetPhysicalDeviceQueueFamilyProperties(device_handle, &queue_families_count, nullptr);

				Vector<VkQueueFamilyProperties> queue_families;
				queue_families.SetSizeUninitialized((SizeT)queue_families_count);
				vkGetPhysicalDeviceQueueFamilyProperties(device_handle, &queue_families_count, queue_families.Data());

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
						queue_graphics_scores[index] += 3;
						queue_transfer_scores[index] -= 1;
						queue_compute_scores[index] -= 1;
					}
					if (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT)
					{
						queue_graphics_scores[index] -= 1;
						queue_transfer_scores[index] += 3;
						queue_compute_scores[index] -= 1;
					}
					if (queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT)
					{
						queue_graphics_scores[index] -= 1;
						queue_transfer_scores[index] -= 1;
						queue_compute_scores[index] += 3;
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
							device.HasGraphicsQueue = true;
							device.GraphicsQueueIndex = index;

							if (!(queue_families[index].queueFlags & VK_QUEUE_TRANSFER_BIT) && !(queue_families[index].queueFlags & VK_QUEUE_COMPUTE_BIT))
								device.IsGraphicsQueueShared = false;
						}
					}
					if (queue_transfer_scores[index] >= best_queue_transfer_score)
					{
						best_queue_transfer_score = queue_transfer_scores[index];
						if (best_queue_transfer_score > 0)
						{
							device.HasTransferQueue = true;
							device.TransferQueueIndex = index;

							if (!(queue_families[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !(queue_families[index].queueFlags & VK_QUEUE_COMPUTE_BIT))
								device.IsGraphicsQueueShared = false;
						}
					}
					if (queue_compute_scores[index] >= best_queue_compute_score)
					{
						best_queue_compute_score = queue_compute_scores[index];
						if (best_queue_compute_score > 0)
						{
							device.HasComputeQueue = true;
							device.ComputeQueueIndex = index;

							if (!(queue_families[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !(queue_families[index].queueFlags & VK_QUEUE_TRANSFER_BIT))
								device.IsGraphicsQueueShared = false;
						}
					}
				}

				if (device.HasGraphicsQueue)
					device_score += DeviceScoreWeight_HasGraphicsQueue;

				if (device.HasTransferQueue)
					device_score += DeviceScoreWeight_HasTransferQueue;

				if (device.HasComputeQueue)
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
				LF_RENDERER_TRACE("    %{}", extension);
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

		s_VulkanData->PhysicalDevice = Utils::PickBestPhysicalDevice(devices);

		// TODO (Avr): Macros.
		if (s_VulkanData->PhysicalDevice.PhysicalDeviceHandle == VK_NULL_HANDLE)
		{
			LF_RENDERER_FATAL("Failed to find a GPU with all the required Vulkan features! Aborting.");
			return false;
		}

		VkPhysicalDeviceProperties gpu_properties = {};
		vkGetPhysicalDeviceProperties(s_VulkanData->PhysicalDevice.PhysicalDeviceHandle, &gpu_properties);
		LF_RENDERER_INFO("Renderer Hardware: %{}", (const char*)(&gpu_properties.deviceName[0]));

		LF_RENDERER_TRACE("Creating the Vulkan Logical Device...");

		Vector<VkDeviceQueueCreateInfo> queue_create_infos = {};
		Array<uint32, 3> required_queue_family_indices = {
			s_VulkanData->PhysicalDevice.GraphicsQueueIndex,
			s_VulkanData->PhysicalDevice.TransferQueueIndex,
			s_VulkanData->PhysicalDevice.ComputeQueueIndex
		};

		for (SizeT index = 0; index < required_queue_family_indices.Size(); index++)
		{
			bool created = false;
			for (SizeT j = 0; j < index; j++)
			{
				if (required_queue_family_indices[index] == required_queue_family_indices[j])
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
		}

		VkDeviceCreateInfo device_create_info = {};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		VkPhysicalDeviceFeatures features = {};
		device_create_info.pEnabledFeatures = &features;

		device_create_info.queueCreateInfoCount = (uint32)queue_create_infos.Size();
		device_create_info.pQueueCreateInfos = queue_create_infos.Data();

		Vector<const char*> device_extensions;
		device_extensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		device_create_info.enabledExtensionCount = (uint32)device_extensions.Size();
		device_create_info.ppEnabledExtensionNames = device_extensions.Data();

		// Deprecated and ignored, so pass nothing.
		device_create_info.enabledLayerCount = 0;
		device_create_info.ppEnabledLayerNames = nullptr;

		LF_VK_CHECK_RETURN(vkCreateDevice(s_VulkanData->PhysicalDevice.PhysicalDeviceHandle, &device_create_info, nullptr, &s_VulkanData->LogicalDevice), false);
		LF_RENDERER_INFO("Vulkan Logical Device created successfully.");

		LF_RENDERER_TRACE("Aquiring the device queue handles...");
		vkGetDeviceQueue(s_VulkanData->LogicalDevice, s_VulkanData->PhysicalDevice.GraphicsQueueIndex, 0, &s_VulkanData->GraphicsQueue);
		vkGetDeviceQueue(s_VulkanData->LogicalDevice, s_VulkanData->PhysicalDevice.TransferQueueIndex, 0, &s_VulkanData->TransferQueue);
		vkGetDeviceQueue(s_VulkanData->LogicalDevice, s_VulkanData->PhysicalDevice.ComputeQueueIndex, 0, &s_VulkanData->ComputeQueue);

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

	Result CreateRenderingContext(Ref<RenderingContext>& out_rendering_context, const RenderingContextSpecification& specification)
	{
		Result result;
		Ref<VulkanContext> context = Ref<VulkanContext>::Create(specification, result);

		if (result == Result::Success)
			out_rendering_context = context.As<RenderingContext>();
		else
			out_rendering_context = nullptr;

		return result;
	}

	VulkanContext::VulkanContext(const RenderingContextSpecification& specification, Result& out_result)
		: RenderingContext(specification)
	{
		out_result = Result::Success;
	}

	VulkanContext::~VulkanContext()
	{
	}

	Result CreateFramebuffer(Ref<Framebuffer>& out_framebuffer, const FrambufferSpecification& specification)
	{
		Result result;
		Ref<VulkanFramebuffer> framebuffer = Ref<VulkanFramebuffer>::Create(specification, result);

		if (result == Result::Success)
			out_framebuffer = framebuffer.As<Framebuffer>();
		else
			out_framebuffer = nullptr;

		return result;
	}

	VulkanFramebuffer::VulkanFramebuffer(const FrambufferSpecification& specification, Result& out_result)
		: Framebuffer(specification)
		, m_Handle(VK_NULL_HANDLE)
	{
		out_result = Result::Success;
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

} } }