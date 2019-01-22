#pragma once

#include "VulkanInstance.h"
//#include "VulkanSettings.hpp"

/*
	Top class from where the whole Vulkan api get controlled
*/
class VulkanBase
{

public:
	VulkanBase();
	~VulkanBase();

private:
	VulkanInstance *m_instance = nullptr;

};