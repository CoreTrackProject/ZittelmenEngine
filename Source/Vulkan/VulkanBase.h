#pragma once

#define NOMINMAX
#include <algorithm>
#include <memory>

#include <QTimer>

#include "VulkanInstance.h"
#include "VulkanDebug.h"
#include "VulkanDevice.h"
#include "VulkanWindow.h"
#include "VulkanSwapchain.h"
#include "VulkanShader.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanCommand.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

/*
	Top class from where the whole Vulkan api get controlled
*/
class VulkanBase
{

public:
	VulkanBase();
	~VulkanBase();

	void setTargetRenderSurface(QWidget *targetWindow);
	void resizeTargetRenderSurface(uint32_t width, uint32_t height);

	void initialize();
	void destroy();
	void renderFrame();

private:
	void init_syncobjects();

	bool enableValidation = false;

	QWidget *targetRenderWindow = nullptr;
	 
	std::unique_ptr<VulkanInstance> instance				 = nullptr;
	std::unique_ptr<VulkanDebug> vulkanDebug				 = nullptr;
	std::unique_ptr<VulkanDevice> vulkanDevice				 = nullptr;
	std::unique_ptr<VulkanWindow> window					 = nullptr;
	std::unique_ptr<VulkanSwapchain> swapchain				 = nullptr;
	std::unique_ptr<VulkanShader> shader					 = nullptr;
	std::unique_ptr<VulkanGraphicsPipeline> graphicsPipeline = nullptr;
	std::unique_ptr<VulkanCommand> command					 = nullptr;

	VkQueue presentQueue = VK_NULL_HANDLE;

	std::vector<VkSemaphore> imageAvailableSemaphoreCollection;
	std::vector<VkSemaphore> renderFinishedSemaphoreCollection;

	std::vector<VkFence> inFlightFences;

	size_t currentFrame = 0;

	bool isRenderActive;
	bool renderRunning = false;
};