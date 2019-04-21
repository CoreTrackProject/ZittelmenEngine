#pragma once

#include <QWidget>
#include <QWindow>
#include <QTimer>

#include <iostream>

#include <stdexcept>

#include "Vulkan/VulkanController.h"

#include "ZiScene.h"

enum ZiEngineStatus {
	Created,
	Initializing,
	Ready,
	Destroyed,
	Resizing,
	Rendering
};

class ZiEngine
{

public:
    ZiEngine();
	~ZiEngine();

	void Initialize();
	void Destroy();

	void SetTargetRenderSurface(QWidget *targetRenderSurface);
	void SetTargetRenderSurface(QWindow *targetRenderSurface);
	void RenderFrame();
	void Resize(std::uint32_t width, std::uint32_t height);
	ZiEngineStatus GetStatus();
	void SetScene(std::shared_ptr<ZiScene> scene);

private:
	std::unique_ptr<VulkanController> vulkanController = nullptr;
	bool isInitialized;
	ZiEngineStatus currStatus;
	std::shared_ptr<ZiScene> currScene;
};
