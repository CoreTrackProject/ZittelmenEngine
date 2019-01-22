#include "VulkanInstance.h"

// This code is used from the qt vulkan examples

VulkanInstance::VulkanInstance(bool enableValidation)
{
	this->init_vkInstance(enableValidation);
}

VulkanInstance::~VulkanInstance()
{
	this->destroy_vkInstance();
}

void VulkanInstance::init_vkInstance(bool enableValidation)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "CoreTrack";
	appInfo.pEngineName = "Zittelmen Engine";
	appInfo.apiVersion = VK_API_VERSION_1_0;

	std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.enabledLayerCount = 0;

#if defined(_WIN32)
	instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(_DIRECT2DISPLAY)
	instanceExtensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
	instanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
	instanceExtensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#endif


	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = NULL;
	instanceCreateInfo.pApplicationInfo = &appInfo;


	if (instanceExtensions.size() > 0)
	{
		if (enableValidation)
		{
			instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}

		instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
	}

	if (enableValidation)
	{

		// On desktop the LunarG loaders exposes a meta layer that contains all layers
		int32_t validationLayerCount = 1;
		const char *validationLayerNames[] = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		instanceCreateInfo.enabledLayerCount = validationLayerCount;
		instanceCreateInfo.ppEnabledLayerNames = validationLayerNames;
	}

	VkResult res = vkCreateInstance(&instanceCreateInfo, nullptr, &this->m_vkInstance);

}

void VulkanInstance::destroy_vkInstance()
{
	vkDestroyInstance(this->m_vkInstance, nullptr);
}
