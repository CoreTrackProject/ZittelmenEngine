#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include <QDebug>

#include "VulkanSwapchain.h"

class VulkanGraphicsPipeline
{

public:
	VulkanGraphicsPipeline(VkDevice &logicalDevice, VkShaderModule &vertexShaderModule, VkShaderModule  &fragmentShaderModule, VkExtent2D &swapchainExtent, VkSurfaceFormatKHR  &swapchainImageFormat, std::vector<Image> &swapchainImageCollection);
	~VulkanGraphicsPipeline();

	std::vector<VkFramebuffer> &getFramebufferCollection();
	VkRenderPass &getRenderPass();
	VkPipeline &getGraphicsPipeline();

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
	
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;
	
	std::vector<Image> &swapchainImageCollection;
	std::vector<VkFramebuffer> swapchainFramebufferCollection;

};