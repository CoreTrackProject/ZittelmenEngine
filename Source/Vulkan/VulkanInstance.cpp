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

VkInstance &VulkanInstance::GetInstance()
{
	return this->m_vkInstance;
}

void VulkanInstance::init_vkInstance(bool enableValidation)
{

	VkApplicationInfo appInfo  = {};
	appInfo.sType			   = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext			   = NULL;
	appInfo.pApplicationName   = "CoreTrack";
	appInfo.applicationVersion = 1;
	appInfo.pEngineName		   = "Zittelmen Engine";
	appInfo.engineVersion      = 1;
	appInfo.apiVersion		   = VK_API_VERSION_1_1;
	
	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	
	if (enableValidation) {
		instanceCreateInfo.pNext = &VulkanDebug::DebugUtilsMessengerCreateInfo;
	} else {
		instanceCreateInfo.pNext = nullptr;
	}

	std::vector<const char*> instanceExtensions;
	instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

	if (instanceExtensions.size() > 0)
	{
		if (enableValidation)
		{
			VulkanDebug::addInstanceDebugExtensionCollection(instanceExtensions);
		}
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
	}

	std::vector<char const *> validationLayers =
		VulkanDebug::addInstanceDebugLayerCollection(std::vector<char const *>());

	if (enableValidation)
	{
		// On desktop the LunarG loaders exposes a meta layer that contains all layers
		instanceCreateInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.ppEnabledLayerNames = nullptr;
	}

	VkResult res = vkCreateInstance(&instanceCreateInfo, NULL, &this->m_vkInstance);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to create vulkan instance.");
	}
}

void VulkanInstance::destroy_vkInstance()
{
	vkDestroyInstance(this->m_vkInstance, nullptr);
}
