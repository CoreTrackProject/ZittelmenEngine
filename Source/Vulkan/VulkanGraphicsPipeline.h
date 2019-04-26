#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include <QDebug>
#include <stdexcept>

#include "VulkanSwapchain.h"
#include "VulkanVertex.hpp"
#include "VulkanUtils.hpp"
#include "VulkanTexture.h"

/*

*/
struct VulkanGraphicsPipelineCreateInfo {
	VkPhysicalDevice      physicalDevice;
	VkDevice              logicalDevice; 
	VkShaderModule        vertexShaderModule;
	VkShaderModule        fragmentShaderModule;
	VkExtent2D            swapchainExtent;
	VkSurfaceFormatKHR    swapchainImageFormat;
	VkDescriptorSetLayout descriptorSetLayout;
};


/*

*/
class VulkanGraphicsPipeline
{

public:
	VulkanGraphicsPipeline(VulkanGraphicsPipelineCreateInfo createInfo);
	~VulkanGraphicsPipeline();

	VkRenderPass &GetRenderPass();
	VkPipeline &GetGraphicsPipeline();
	VkPipelineLayout &GetGraphicsPipelineLayout();

private:
	void init_graphicsPipelineLayout();
	void init_renderpass();

private:
	VulkanGraphicsPipelineCreateInfo createInfo = {};

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkRenderPass renderPass         = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline     = VK_NULL_HANDLE;
	
};