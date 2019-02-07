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
#include "VulkanRuntime.h"
#include "VulkanFactory.h"


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
	std::unique_ptr<VulkanRuntime> runtime				     = nullptr;


	std::unique_ptr<VulkanFactory> factory				     = nullptr;

	VulkanVertex vertex;

};