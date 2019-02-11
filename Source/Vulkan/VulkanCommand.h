#pragma once

#include <Vulkan/vulkan.h>

#include "VulkanDevice.h"
#include "Vulkan/VulkanVertex.hpp"

class VulkanCommand {

public:
	VulkanCommand(VkPhysicalDevice &physicalDev, VkDevice &logicalDevice, DeviceInfo &deviceInfo, std::vector<VkFramebuffer> &frameBufferCollection, VkRenderPass &renderpass, VkExtent2D &swapchainExtent, VkPipeline &graphicsPipeline, VkQueue transferQueue);
	~VulkanCommand();


	std::vector<VkCommandBuffer> &getDrawCommandBufferCollection();
	void uploadVertexData(std::vector<Vertex> &vertexData);


private:
	void init_commandPool();
	void init_drawCommand();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
	DeviceInfo &deviceInfo;
	VkDevice &logicalDevice;
	VkPhysicalDevice &physicalDev;
	VkRenderPass &renderpass;
	VkExtent2D &swapchainExtent;
	VkPipeline &graphicsPipeline;
	std::vector<VkFramebuffer> &frameBufferCollection;
	
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	uint32_t vertexCount;
	VkQueue &transferQueue;

	VkCommandPool commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> drawCommandBufferCollection;
	
};