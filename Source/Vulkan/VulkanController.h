#pragma once

#define NOMINMAX
#include <algorithm>
#include <memory>


#include <QTimer>
#include <QImage>


#include "VulkanInstance.h"
#include "VulkanDebug.h"
#include "VulkanDevice.h"
#include "VulkanWindow.h"
#include "VulkanSwapchain.h"
#include "VulkanShader.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanCommand.h"
#include "VulkanRuntime.h"
#include "VulkanUniform.h"
#include "VulkanVertex.hpp"


enum VulkanControllerStatus {
	VC_Created,
	VC_Initializing,
	VC_Ready,
	VC_Rendering,
	VC_Resizing
};


/*
	Top class from where the whole Vulkan api get controlled
*/
class VulkanController
{
public:
	VulkanController();
	~VulkanController();

	void SetTargetRenderSurface(WId targetWindow, std::uint32_t width, std::uint32_t height);
	void ResizeTargetRenderSurface(uint32_t width, uint32_t height);

	void Initialize();
	void Destroy();

	void RenderFrame();
	void ImportData(std::vector<VulkanVertex> vertexCollection, std::vector<std::uint32_t> indexCollection, std::shared_ptr<QImage> meshTexture);

private:
	void initVulkanInstance();
	void destroyVulkanInstance();

	void initVulkanDebug();
	void destroyVulkanDebug();

	void initVulkanDevice();
	void destroyVulkanDevice();

	void initVulkanWindow();
	void destroyVulkanWindow();

	void initVulkanSwapchain();
	void destroyVulkanSwapchain();

	void initVulkanShader();
	void destroyVulkanShader();
	
	void initVulkanGraphicsPipeline();
	void destroyVulkanGraphicsPipeline();

	void initVulkanCommand();
	void destroyVulkanCommand();

	void uploadContent();

	void initVulkanRuntime();
	void destroyVulkanRuntime();

	void initVulkanUniform();
	void destroyVulkanUniform();


private:
	bool enableValidation  = false;
	WId target;
	std::uint32_t width = 0, height = 0;

	std::unique_ptr<VulkanInstance> instance				 = nullptr;
	std::unique_ptr<VulkanDebug> vulkanDebug				 = nullptr;
	std::unique_ptr<VulkanDevice> vulkanDevice				 = nullptr;
	std::unique_ptr<VulkanWindow> window					 = nullptr;
	std::unique_ptr<VulkanSwapchain> swapchain				 = nullptr;
	std::unique_ptr<VulkanShader> shader					 = nullptr;
	std::unique_ptr<VulkanGraphicsPipeline> graphicsPipeline = nullptr;
	std::unique_ptr<VulkanCommand> command					 = nullptr;
	std::unique_ptr<VulkanRuntime> runtime				     = nullptr;
	std::shared_ptr<VulkanUniform> uniform					 = nullptr;



	// Create a scene class which holds all the imported contetnt
	std::shared_ptr<QImage> imageData = nullptr;
	std::shared_ptr<VulkanTexture> imageTexture = nullptr;

	std::vector<VulkanVertex> vertexCollection;
	std::vector<std::uint32_t> indexCollection;

	VulkanControllerStatus currStatus;

};