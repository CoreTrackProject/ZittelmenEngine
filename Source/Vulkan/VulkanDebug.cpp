#include "VulkanDebug.h"


VulkanDebug::VulkanDebug(VulkanDebugCreateInfo createInfo) : createInfo(createInfo)
{
	this->init_vulkanDebug();
}

VulkanDebug::~VulkanDebug()
{
	this->destroy_vulkanDebug();
}

void VulkanDebug::AddInstanceDebugExtensionCollection(std::vector<const char*> *extensionCollection)
{
	if (extensionCollection == nullptr) {
		return;
	}

	if (extensionCollection->empty() || 
		std::find(extensionCollection->begin(), extensionCollection->end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == extensionCollection->end())
	{
		extensionCollection->push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
}

void VulkanDebug::AddInstanceDebugLayerCollection(std::vector<const char*> *extensionCollection)
{
	if (extensionCollection == nullptr) {
		return;
	}

	if (extensionCollection->empty() ||
		std::find(extensionCollection->begin(), extensionCollection->end(), "VK_LAYER_LUNARG_standard_validation") == extensionCollection->end())
	{
		extensionCollection->push_back("VK_LAYER_LUNARG_standard_validation");
	}
}

void VulkanDebug::init_vulkanDebug()
{
	this->CreateDebugUtilsMessengerEXT  = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(this->createInfo.instance, "vkCreateDebugUtilsMessengerEXT"));
	this->DestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(this->createInfo.instance, "vkDestroyDebugUtilsMessengerEXT"));

	if (CreateDebugUtilsMessengerEXT(this->createInfo.instance, &VulkanDebugGlobal::DebugUtilsMessengerCreateInfo, nullptr, &this->debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to set up debug messenger.");
	}

}

void VulkanDebug::destroy_vulkanDebug()
{
	this->DestroyDebugUtilsMessengerEXT(this->createInfo.instance, this->debugMessenger, nullptr);
}
