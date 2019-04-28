#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <stdexcept>

#include <QDebug>

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
	VkRenderPass		  renderpass;
};


/*
	
*/
class VulkanGraphicsPipeline
{

public:
	VulkanGraphicsPipeline(VulkanGraphicsPipelineCreateInfo createInfo);
	~VulkanGraphicsPipeline();

	VkPipeline &GetGraphicsPipeline();
	VkPipelineLayout &GetGraphicsPipelineLayout();

private:
	void init_graphicsPipelineLayout();

private:
	VulkanGraphicsPipelineCreateInfo createInfo = {};

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline     = VK_NULL_HANDLE;
	
};