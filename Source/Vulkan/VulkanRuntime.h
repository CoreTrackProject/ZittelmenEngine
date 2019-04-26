#pragma once

#include <vector>
#include <QDebug>
#include <Vulkan\vulkan.h>
#include <functional>

#include "VulkanUniform.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

/*

*/
struct VulkanRuntimeCreateInfo {
	VkDevice					   logicalDevice;
	VkSwapchainKHR				   swapchain;
	std::vector<VkCommandBuffer>   commandBufferCollection;
	VkQueue						   graphicsQueue;
	VkQueue						   presentQueue;
	std::shared_ptr<VulkanUniform> uniform;
};

/*

*/
class VulkanRuntime {

public:
	VulkanRuntime(VulkanRuntimeCreateInfo createInfo);
	~VulkanRuntime();

	void RenderFrame();
	size_t getCurrentFrameIdx();

private:
	void destroy();
	void init_syncobjects();
	
private:
	VulkanRuntimeCreateInfo createInfo = {};

	std::vector<VkSemaphore> imageAvailableSemaphoreCollection;
	std::vector<VkSemaphore> renderFinishedSemaphoreCollection;
	std::vector<VkFence> inFlightFences;

	size_t currentFrameIdx = 0;
	bool renderingFailed = false;

};