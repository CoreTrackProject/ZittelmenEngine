#include "VulkanDebug.h"

namespace debug {
	VkDebugUtilsMessengerEXT debugMessenger;


	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		qDebug() << "Debug utils data: " << messageType;

		//std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	//==============================

	VkDebugReportCallbackEXT debugReport;

	VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t srcObject,
		size_t location,
		int32_t msgCode,
		const char* pLayerPrefix,
		const char* pMsg,
		void* pUserData)
	{
		// Select prefix depending on flags passed to the callback
		// Note that multiple flags may be set for a single validation message
		std::string prefix("");

		// Error that may result in undefined behaviour
		if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		{
			prefix += "ERROR:";
		};
		// Warnings may hint at unexpected / non-spec API usage
		if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		{
			prefix += "WARNING:";
		};
		// May indicate sub-optimal usage of the API
		if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
		{
			prefix += "PERFORMANCE:";
		};
		// Informal messages that may become handy during debugging
		if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
		{
			prefix += "INFO:";
		}
		// Diagnostic info from the Vulkan loader and layers
		// Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
		if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
		{
			prefix += "DEBUG:";
		}

		// Display message to default output (console/logcat)

		qDebug() << prefix.c_str() << " [" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg;


		/* if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
			 std::cerr << debugMessage.str() << "\n";
		 }
		 else {
			 std::cout << debugMessage.str() << "\n";
		 }*/


		 //fflush(stdout);

		 // The return value of this callback controls wether the Vulkan call that caused
		 // the validation message will be aborted or not
		 // We return VK_FALSE as we DON'T want Vulkan calls that cause a validation message 
		 // (and return a VkResult) to abort
		 // If you instead want to have calls abort, pass in VK_TRUE and the function will 
		 // return VK_ERROR_VALIDATION_FAILED_EXT 
		return VK_FALSE;
	}


}

VulkanDebug::VulkanDebug(VkInstance *instance)
{
	this->instance = instance;
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
		//extensionCollection.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
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
	this->CreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(*this->instance, "vkCreateDebugUtilsMessengerEXT"));
	this->DestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(*this->instance, "vkDestroyDebugUtilsMessengerEXT"));


	VkDebugUtilsMessengerCreateInfoEXT dbg_messenger_create_info;

	// VK_EXT_debug_utils style
	dbg_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	dbg_messenger_create_info.pNext = NULL;
	dbg_messenger_create_info.flags = 0;
	dbg_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	dbg_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	dbg_messenger_create_info.pfnUserCallback = debug::debugCallback;

	// TODO
	//dbg_messenger_create_info.pUserData = demo;
	//inst_info.pNext = &dbg_messenger_create_info;




	if (CreateDebugUtilsMessengerEXT(*this->instance, &dbg_messenger_create_info, nullptr, &debug::debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}

	// ====================================


	//this->CreateDebugReportCallbackEXT  = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>( vkGetInstanceProcAddr(*this->instance, "vkCreateDebugReportCallbackEXT"));
	//this->DestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(*this->instance, "vkDestroyDebugReportCallbackEXT"));
	//



	//VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = {};
	//dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	//dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)debug::messageCallback;
	//dbgCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT;

	//VkResult err = this->CreateDebugReportCallbackEXT(
	//	*this->instance,
	//	&dbgCreateInfo,
	//	nullptr,
	//	&debug::debugReport);
	//assert(!err);


}

void VulkanDebug::destroy_vulkanDebug()
{
	this->DestroyDebugUtilsMessengerEXT(*this->instance, debug::debugMessenger, nullptr);
	//this->DestroyDebugReportCallbackEXT(*this->instance, debug::debugReport,   nullptr);
}
