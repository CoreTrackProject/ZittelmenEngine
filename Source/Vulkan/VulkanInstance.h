#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <vector>

#include "VulkanDebug.h"

class VulkanInstance
{

public:
	VulkanInstance(bool enableValidation);
	~VulkanInstance();

	VkInstance* getInstance();

private:
	void init_vkInstance(bool enableValidation);
	void destroy_vkInstance();
	
private:
	VkInstance m_vkInstance = VK_NULL_HANDLE;

};