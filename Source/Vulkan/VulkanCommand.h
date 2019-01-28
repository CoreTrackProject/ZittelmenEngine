#pragma once

#include <Vulkan/vulkan.h>
#include "VulkanDevice.h"

class VulkanCommand {

public:
	VulkanCommand(VkDevice* logicalDevice, DeviceInfo* deviceInfo, std::vector<VkFramebuffer>* frameBufferCollection, VkRenderPass* renderpass, VkExtent2D* swapchainExtent, VkPipeline* graphicsPipeline);
	~VulkanCommand();

	void init_commandPool();
	void init_commandBuffer();

private:
	VkCommandPool commandPool    = VK_NULL_HANDLE;
	DeviceInfo* deviceInfo       = nullptr;
	VkDevice* logicalDevice      = nullptr;
	VkRenderPass* renderpass     = nullptr;
	VkExtent2D* swapchainExtent  = nullptr;
	VkPipeline* graphicsPipeline = nullptr;

	std::vector<VkCommandBuffer> commandBufferCollection;
	std::vector<VkFramebuffer>*  frameBufferCollection;

};