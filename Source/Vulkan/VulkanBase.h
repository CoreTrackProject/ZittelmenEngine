#pragma once

#include "VulkanInstance.h"
#include "VulkanDebug.h"

/*
	Top class from where the whole Vulkan api get controlled
*/
class VulkanBase
{

public:
	VulkanBase();
	~VulkanBase();

private:
	bool enableValidation = false;

	VulkanInstance *m_instance = nullptr;
	VulkanDebug *m_vulkanDebug = nullptr;

};