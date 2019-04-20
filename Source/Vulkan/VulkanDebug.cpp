#include "VulkanDebug.h"


VulkanDebug::VulkanDebug(VkInstance &instance) : instance(instance)
{
	this->init_vulkanDebug();
}

VulkanDebug::~VulkanDebug()
{
	this->destroy_vulkanDebug();
}

std::vector<const char*> VulkanDebug::addInstanceDebugExtensionCollection(std::vector<const char*> &extensionCollection)
{
	if (extensionCollection.empty() || 
		std::find(extensionCollection.begin(), extensionCollection.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == extensionCollection.end())
	{
		extensionCollection.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensionCollection;
}

std::vector<const char*> VulkanDebug::addInstanceDebugLayerCollection(std::vector<const char*> &extensionCollection)
{
	std::vector<char const *> validationLayers{
			"VK_LAYER_LUNARG_standard_validation"
	};
	return validationLayers;
}

void VulkanDebug::init_vulkanDebug()
{
	this->CreateDebugUtilsMessengerEXT  = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(this->instance, "vkCreateDebugUtilsMessengerEXT"));
	this->DestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(this->instance, "vkDestroyDebugUtilsMessengerEXT"));

	if (CreateDebugUtilsMessengerEXT(this->instance, &VulkanDebug::DebugUtilsMessengerCreateInfo, nullptr, &this->debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to set up debug messenger.");
	}

}

void VulkanDebug::destroy_vulkanDebug()
{
	this->DestroyDebugUtilsMessengerEXT(this->instance, this->debugMessenger, nullptr);
}
