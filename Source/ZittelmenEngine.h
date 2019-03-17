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

	void initialize();
	void destroy();

	bool getInitializedStatus();

	void setTargetRenderSurface(QWidget *targetRenderSurface);
	void setTargetRenderSurface(QWindow *targetRenderSurface);
	
	void renderFrame();

	void resize(uint32_t width, uint32_t height);

	

private:
	std::unique_ptr<VulkanController> vulkanController = nullptr;
	bool isInitialized;
};
