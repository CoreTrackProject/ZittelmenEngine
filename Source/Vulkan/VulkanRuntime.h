#pragma once

#include <vector>
#include <QDebug>
#include <Vulkan\vulkan.h>
#include <functional>

const int MAX_FRAMES_IN_FLIGHT = 2;

class VulkanRuntime {

public:
	VulkanRuntime(VkDevice &logicalDevice, VkSwapchainKHR &swapchain, std::vector<VkCommandBuffer> &commandBufferCollection, VkQueue &graphicsQueue, VkQueue &presentQueue);
	~VulkanRuntime();

	void renderFrame();
	size_t getCurrentFrameIdx();

	void registerUpdateUBOCallback(std::function<void(uint32_t)> updateUBO);

private:
	void destroy();
	void init_syncobjects();
	
private:
	VkDevice &logicalDevice;
	VkQueue &graphicsQueue;
	VkQueue &presentQueue;

	VkSwapchainKHR &swapchain;
	std::vector<VkCommandBuffer> &commandBufferCollection;

	std::vector<VkSemaphore> imageAvailableSemaphoreCollection;
	std::vector<VkSemaphore> renderFinishedSemaphoreCollection;
	std::vector<VkFence> inFlightFences;

	size_t currentFrameIdx = 0;
	bool isRenderActive;
	bool renderRunning = false;

	std::function<void(uint32_t)> updateUBO;

};