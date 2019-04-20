#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include <QDebug>
#include <stdexcept>

#include "VulkanSwapchain.h"
#include "VulkanVertex.hpp"
#include "VulkanUtils.hpp"
#include "VulkanTexture.h"

class VulkanGraphicsPipeline
{

public:
	VulkanGraphicsPipeline(VkPhysicalDevice &physicalDevice, VkDevice &logicalDevice, VkShaderModule &vertexShaderModule, VkShaderModule  &fragmentShaderModule, VkExtent2D &swapchainExtent, VkSurfaceFormatKHR  &swapchainImageFormat, std::vector<Image> &swapchainImageCollection, VkDescriptorSetLayout &descriptorSetLayout, VkImageView &depthImageView);
	~VulkanGraphicsPipeline();

	std::vector<VkFramebuffer> &GetFramebufferCollection();
	VkRenderPass &GetRenderPass();
	VkPipeline &GetGraphicsPipeline();
	VkPipelineLayout &GetGraphicsPipelineLayout();

private:
	void init_graphicsPipelineLayout();
	void init_renderpass();
	void init_framebuffer();

private:
	VkShaderModule &vertexShaderModule;
	VkShaderModule &fragmentShaderModule;
	VkExtent2D &swapchainExtent2D;
	VkSurfaceFormatKHR &swapchainImageFormat;
	VkDevice &logicalDevice;
	VkDescriptorSetLayout &descriptorSetLayout;
	VkImageView &depthImageView;
	VkPhysicalDevice &physicalDevice;


	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;
	
	std::vector<Image> &swapchainImageCollection;
	std::vector<VkFramebuffer> swapchainFramebufferCollection;


};