#pragma once

#include <QWidget>
#include <QWindow>
#include <QTimer>

#include <iostream>

#include "Vulkan/VulkanController.h"

class ZittelmenEngine
{

public:
    ZittelmenEngine();
	~ZittelmenEngine();

	void setTargetRenderSurface(QWidget *targetRenderSurface);
	void setTargetRenderSurface(QWindow *targetRenderSurface);
	
	void renderFrame();
	bool getInitializedStatus();

	void initialize();
	void destroy();

	void resize(uint32_t width, uint32_t height);

private:
	std::unique_ptr<VulkanController> vulkanController = nullptr;
	bool isInitialized;
};
