#pragma once

#include <QVulkanInstance>
#include <QLoggingCategory>

#include <iostream>

#include "Vulkan/VulkanWindow.h"
#include "Vulkan/VulkanRenderer.h"

#include "Qt3D/Qt3DWindow.h"


class ZittelmenEngine
{

public:
    ZittelmenEngine();
	VulkanWindow* getVulkanWindow();
	Qt3DWindow* getQt3DWindow();


private:
	VulkanWindow *m_vulkanWindow;
	QVulkanInstance m_vulkanInstance;
	Qt3DWindow *renderer;
};
