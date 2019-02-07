#pragma once

#include <Vulkan/vulkan.h>
#include "VulkanDevice.h"

class VulkanCommand {

public:
	VulkanCommand(VkDevice &logicalDevice, DeviceInfo &deviceInfo, std::vector<VkFramebuffer> &frameBufferCollection, VkRenderPass &renderpass, VkExtent2D &swapchainExtent, VkPipeline &graphicsPipeline, VkBuffer &vertexBuffer, uint32_t vertexCount);
	~VulkanCommand();

	std::vector<VkCommandBuffer> &getCommandBufferCollection();

private:
	void init_commandPool();
	void init_commandBuffer();

private:
	DeviceInfo &deviceInfo;
	VkDevice &logicalDevice;
	VkRenderPass &renderpass;
	VkExtent2D &swapchainExtent;
	VkPipeline &graphicsPipeline;
	std::vector<VkFramebuffer> &frameBufferCollection;
	VkBuffer &vertexBuffer;
	uint32_t vertexCount;

	VkCommandPool commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> commandBufferCollection;
	

};