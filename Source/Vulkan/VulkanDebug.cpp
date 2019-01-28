#include "VulkanDebug.h"

namespace debug {
	VkDebugReportCallbackEXT msgCallback;

	VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackEXT(
		VkDebugReportFlagsEXT                       msgFlags,
		VkDebugReportObjectTypeEXT                  objectType,
		uint64_t                                    object,
		size_t                                      location,
		int32_t                                     messageCode,
		const char*                                 pLayerPrefix,
		const char*                                 pMessage,
		void*                                       pUserData)
	{
		

		if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
			std::cout << "ERROR: " << std::endl;
		}
		else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
			std::cout << "WARNING: " << std::endl;
		}
		else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
			std::cout << "PERFORMANCE WARNING: " << std::endl;
		}
		else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
			std::cout << "INFO: " << std::endl;
		}
		else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
			std::cout << "DEBUG: " << std::endl;
		}
		std::cout << "[" << pLayerPrefix << "] Code " << messageCode << " : " << pMessage;


		return VK_FALSE;
	}

}


VulkanDebug::VulkanDebug(VkInstance *instance)
{
	this->instance = instance;
	this->init_vulkanDebug();
}

void VulkanDebug::init_vulkanDebug()
{

	CreateDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(*this->instance,   "vkCreateDebugReportCallbackEXT"));
	DestroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(*this->instance, "vkDestroyDebugReportCallbackEXT"));


	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)debug::debugReportCallbackEXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;

	VkResult res = CreateDebugReportCallback(*this->instance, &createInfo, nullptr, &debug::msgCallback);
}

void VulkanDebug::destroy_vulkanDebug()
{
	DestroyDebugReportCallback(*this->instance, debug::msgCallback, nullptr);
}
