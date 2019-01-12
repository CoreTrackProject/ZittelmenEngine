#pragma once

#include <Vulkan/vulkan.h>
#include <QFile>

#include <iostream>

// This code is used from the qt vulkan examples

class VulkanRenderer
{
public:
    VulkanRenderer();
	~VulkanRenderer();


private:
	void initVkLayers();
	void initVkInstance();

private:
	VkInstance m_vkInstance;

};

