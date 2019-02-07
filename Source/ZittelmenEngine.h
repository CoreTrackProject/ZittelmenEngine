#pragma once

#include <iostream>
#include <QWidget>
#include <QTimer>

#include "Vulkan/VulkanController.h"

class ZittelmenEngine
{

public:
    ZittelmenEngine();
	~ZittelmenEngine();

	void setTargetRenderSurface(QWidget *targetRenderSurface);
	
	void renderFrame();
	bool getInitializedStatus();

	void initialize();
	void destroy();

	void resize(uint32_t width, uint32_t height);

private:
	std::unique_ptr<VulkanController> vulkanController = nullptr;
	VulkanWindow *m_vulkanWindow = nullptr;
	bool isInitialized;
};
