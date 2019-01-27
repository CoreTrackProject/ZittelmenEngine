#pragma once

#include <iostream>
#include <QWidget>

#include "Vulkan/VulkanBase.h"


class ZittelmenEngine
{

public:
    ZittelmenEngine();
	~ZittelmenEngine();

	void setTargetRenderSurface(QWidget *targetRenderSurface);
	void initVulkanRenderer();

private:
	VulkanBase *m_vulkanBase     = nullptr;
	QWidget *targetRenderSurface = nullptr;
	VulkanWindow *m_vulkanWindow = nullptr;
};
