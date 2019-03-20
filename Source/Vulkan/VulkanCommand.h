#pragma once

#include <Vulkan/vulkan.h>
#include <QPixmap>

#include "VulkanVertex.hpp"

#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanTexture.h"

class VulkanCommand {

public:
	VulkanCommand(VkPhysicalDevice &physicalDev, 
		VkDevice &logicalDevice, 
		DeviceInfo &deviceInfo, 
		std::vector<VkFramebuffer> &frameBufferCollection, 
		VkRenderPass &renderpass, 
		VkExtent2D &swapchainExtent, 
		VkPipeline &graphicsPipeline, 
		VkPipelineLayout &pipelineLayout, 
		VkQueue &transferQueue, 
		std::vector<VkDescriptorSet> &descriptorSetCollection
	);
	~VulkanCommand();

	std::vector<VkCommandBuffer> &getDrawCommandBufferCollection();
	void uploadVertexData(std::vector<VulkanVertex> &vertexData, std::vector<uint16_t> &indexCollection);
	void uploadImage(VulkanTexture vulkanTexture);

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
	VkQueue &transferQueue;

	std::shared_ptr<VulkanBuffer> vertexBuffer;
	std::shared_ptr<VulkanBuffer> indexBuffer;

	std::shared_ptr<VulkanTexture> imageTexture;

	uint32_t vertexCount;
	uint16_t indexCount;


	VkCommandPool commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> drawCommandBufferCollection;
	
};