#pragma once

#include "VulkanInstance.h"
#include "VulkanDebug.h"
#include "VulkanDevice.h"
#include "VulkanWindow.h"
#include "VulkanSwapchain.h"
#include "VulkanShader.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanCommand.h"

/*
	Top class from where the whole Vulkan api get controlled
*/
class VulkanBase
{

public:
	VulkanBase();
	~VulkanBase();


	void setTargetRenderSurface(QWidget *hwnd);
	void init();
	

private:
	bool enableValidation = false;

	VulkanInstance* instance    = nullptr;
	VulkanDebug* vulkanDebug    = nullptr;
	VulkanDevice* vulkanDevice  = nullptr;

	QWidget* targetRenderWindow = nullptr;
	VulkanWindow* window	    = nullptr;
	VulkanSwapchain* swapchain  = nullptr;
	VulkanShader* shader	    = nullptr;
	VulkanGraphicsPipeline* graphicsPipeline = nullptr;
	VulkanCommand* command		= nullptr;
};