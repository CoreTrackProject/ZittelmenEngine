#pragma once

#include "VulkanInstance.h"
#include "VulkanDebug.h"
#include "VulkanDevice.h"

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

	VulkanInstance *instance   = nullptr;
	VulkanDebug *vulkanDebug   = nullptr;
	VulkanDevice *vulkanDevice = nullptr;


};