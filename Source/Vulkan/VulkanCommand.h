#pragma once

#include <Vulkan/vulkan.h>
#include <QPixmap>
#include <QByteArray>

#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanTexture.h"

#include "VulkanUtils.hpp"
#include "VulkanVertex.hpp"


struct VulkanCommandCreateInfo {
	VkPhysicalDevice             physicalDev;
	VkDevice                     logicalDevice;
	DeviceInfo                   deviceInfo;
	std::vector<VkFramebuffer>   frameBufferCollection;
	VkRenderPass                 renderpass;
	VkExtent2D                   swapchainExtent;
	VkPipeline                   graphicsPipeline;
	VkPipelineLayout             pipelineLayout;
	VkQueue                      graphicsQueue;
	std::vector<VkDescriptorSet> descriptorSetCollection;
};

/*
	TODO: Update function uploadVertexData with the new beginSingleTimeCommands function
*/
class VulkanCommand {

public:
	VulkanCommand(VulkanCommandCreateInfo createInfo);
	~VulkanCommand();

	std::vector<VkCommandBuffer> GetDrawCommandBufferCollection();
	void UploadVertexData(std::vector<VulkanVertex> &vertexData, std::vector<std::uint32_t> &indexCollection);
    void UploadImage(std::shared_ptr<VulkanTexture> &vulkanTexture);
	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void UpdateFramebufferCollection(std::vector<VkFramebuffer> frameBufferCollection, VkExtent2D newSwapchainExtent);
	void UpdateRenderpass(VkRenderPass renderpass);
private:
	void init_commandPool();

	// TODO: Move those functions to other location (VulkanTexture)
	void copyBufferToImage(VkBuffer buffer, VkImage image, std::uint32_t width, std::uint32_t height);

private:
	VulkanCommandCreateInfo createInfo;
	std::shared_ptr<VulkanBuffer> vertexBuffer;
	std::shared_ptr<VulkanBuffer> indexBuffer;
	std::shared_ptr<VulkanTexture> imageTexture;
	std::uint32_t vertexCount;
	std::uint32_t indexCount;
	VkCommandPool commandPool = VK_NULL_HANDLE;

};