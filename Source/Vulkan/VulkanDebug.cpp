#include "VulkanDebug.h"

namespace debug {
	VkDebugReportCallbackEXT msgCallback;

	VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackEXT(
		VkDebugReportFlagsEXT                       flags,
		VkDebugReportObjectTypeEXT                  objectType,
		uint64_t                                    object,
		size_t                                      location,
		int32_t                                     messageCode,
		const char*                                 pLayerPrefix,
		const char*                                 pMessage,
		void*                                       pUserData)
	{

		return VK_FALSE;
	}

}


VulkanDebug::VulkanDebug(VkInstance *instance)
{
	this->m_instance = instance;
	this->init_vulkanDebug();
}

void VulkanDebug::init_vulkanDebug()
{

	CreateDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(*this->m_instance,   "vkCreateDebugReportCallbackEXT"));
	DestroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(*this->m_instance, "vkDestroyDebugReportCallbackEXT"));


	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)debug::debugReportCallbackEXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT;

	VkResult res = CreateDebugReportCallback(*this->m_instance, &createInfo, nullptr, &debug::msgCallback);
}

void VulkanDebug::destroy_vulkanDebug()
{
	DestroyDebugReportCallback(*this->m_instance, debug::msgCallback, nullptr);
}
