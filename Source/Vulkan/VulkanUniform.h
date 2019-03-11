#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>


#include "VulkanBuffer.h"

// https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class VulkanUniform {

public:
	VulkanUniform(VkPhysicalDevice &physicalDevice, VkDevice &logicalDevice, uint32_t swapChainImageCollectionSize);
	~VulkanUniform();

	VkDescriptorSetLayout &getDescriptorSetLayout();
	std::vector<VkDescriptorSet> &getDescriptorSetCollection();

private:
	void initUniformBuffer();
	void destroyUniformBuffer();
	void initDescriptorPool();
	void initDescriptorSetLayout();
	void destroyDescriptorSetLayout();
	void initDescriptorSet();

private:
	VkDevice &logicalDevice;
	VkPhysicalDevice &physicalDevice;


	VkDescriptorSetLayout descriptorSetLayout; 
	uint32_t swapChainImageCollectionSize;
	std::vector<VulkanBuffer*> uniformBufferCollection;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSetCollection;

};