#pragma once

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <vector>
#include <chrono>
#include <array>

#include "VulkanBuffer.h"

//https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

class VulkanUniform {

public:
	VulkanUniform(VkPhysicalDevice &physicalDevice, VkDevice &logicalDevice, uint32_t swapChainImageCollectionSize, VkExtent2D &swapchainExtent, VkImageView &imageView, VkSampler &imageSampler);
	~VulkanUniform();

	VkDescriptorSetLayout &getDescriptorSetLayout();
	std::vector<VkDescriptorSet> &getDescriptorSetCollection();

	void updateUniformData(uint32_t currFrameIdx);

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
	VkExtent2D &swapchainExtent;
	VkImageView &imageView;
	VkSampler &imageSampler;

	VkDescriptorSetLayout descriptorSetLayout; 
	uint32_t swapChainImageCollectionSize;
	std::vector<std::shared_ptr<VulkanBuffer>> uniformBufferCollection;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSetCollection;

};