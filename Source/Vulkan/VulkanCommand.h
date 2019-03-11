#pragma once

#include <Vulkan/vulkan.h>

#include "VulkanDevice.h"
#include "Vulkan/VulkanVertex.hpp"

#include "VulkanBuffer.h"

class VulkanCommand {

public:
	VulkanCommand(VkPhysicalDevice &physicalDev, 
		VkDevice &logicalDevice, 
		DeviceInfo &deviceInfo, 
		std::vector<VkFramebuffer> &frameBufferCollection, 
		VkRenderPass &renderpass, 
		VkExtent2D &swapchainExtent, 
		VkPipeline &graphicsPipeline, 
		VkPipelineLayout pipelineLayout, 
		VkQueue transferQueue, 
		std::vector<VkDescriptorSet> &descriptorSetCollection
	);
	~VulkanCommand();

	std::vector<VkCommandBuffer> &getDrawCommandBufferCollection();
	void uploadVertexData(std::vector<VulkanVertex> &vertexData, std::vector<uint16_t> &indexCollection);


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
	VkPipelineLayout &pipelineLayout;
	std::vector<VkDescriptorSet> &descriptorSetCollection;
	
	std::unique_ptr<VulkanBuffer> vertexBuffer;
	std::unique_ptr<VulkanBuffer> indexBuffer;

	uint32_t vertexCount;
	uint16_t indexCount;

	VkQueue &transferQueue;

	VkCommandPool commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> drawCommandBufferCollection;
	
};