#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <vector>

#include "VulkanDebug.h"

/*

*/
struct VulkanInstanceCreateInfo {
	bool enableValidation;
};


/*

*/
class VulkanInstance
{

public:
	VulkanInstance(VulkanInstanceCreateInfo createInfo);
	~VulkanInstance();

	VkInstance GetInstance();

private:
	void init_vkInstance(bool enableValidation);
	void destroy_vkInstance();
	
private:
	VulkanInstanceCreateInfo createInfo = {};

	VkInstance m_vkInstance = VK_NULL_HANDLE;

};