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
	void renderFrame();
	void resize(uint32_t width, uint32_t height);

private:
	VulkanBase *m_vulkanBase     = nullptr;
	QWidget *targetRenderSurface = nullptr;
	VulkanWindow *m_vulkanWindow = nullptr;
	bool isInitialized;
};
