#pragma once

#include <QVulkanInstance>
#include <QLoggingCategory>

#include <iostream>

#include "Vulkan/VulkanWindow.h"
#include "Vulkan/VulkanRenderer.h"

class ZittelmenEngine
{

public:
    ZittelmenEngine();
	VulkanWindow* getVulkanWindow();

private:
	VulkanWindow *m_vulkanWindow;
	QVulkanInstance m_vulkanInstance;


};
