#pragma once

#include <Vulkan/vulkan.h>
#include "VulkanDevice.h"

class VulkanCommand {

public:
	VulkanCommand(VkPhysicalDevice &physicalDev, VkDevice &logicalDevice, DeviceInfo &deviceInfo, std::vector<VkFramebuffer> &frameBufferCollection, VkRenderPass &renderpass, VkExtent2D &swapchainExtent, VkPipeline &graphicsPipeline, VkBuffer &vertexBuffer, uint32_t vertexCount, VkQueue transferQueue);
	~VulkanCommand();

	std::vector<VkCommandBuffer> &getCommandBufferCollection();

	void uploadBuffer(VkBuffer &destinationBuffer, VkDeviceSize bufferSize);



private:
	void init_commandPool();
	void init_commandBuffer();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
	DeviceInfo &deviceInfo;
	VkDevice &logicalDevice;
	VkPhysicalDevice &physicalDev;
	VkRenderPass &renderpass;
	VkExtent2D &swapchainExtent;
	VkPipeline &graphicsPipeline;
	std::vector<VkFramebuffer> &frameBufferCollection;
	VkBuffer &vertexBuffer;
	uint32_t vertexCount;

	VkQueue &transferQueue;

	VkCommandPool commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> commandBufferCollection;
	

};