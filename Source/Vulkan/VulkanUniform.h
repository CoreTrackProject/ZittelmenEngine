#pragma once

#include <vulkan/vulkan.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <vector>
#include <chrono>
#include <array>

#include "VulkanBuffer.h"
#include "VulkanUtils.hpp"

//https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

class VulkanUniform {

public:
	VulkanUniform(VkPhysicalDevice &physicalDevice, VkDevice &logicalDevice, uint32_t swapChainImageCollectionSize, VkImageView &imageView, VkSampler &imageSampler, std::uint32_t width, std::uint32_t height);
	~VulkanUniform();

	VkDescriptorSetLayout &GetDescriptorSetLayout();
	std::vector<VkDescriptorSet> &GetDescriptorSetCollection();

	void updateUniformData(uint32_t currFrameIdx);

	void UpdateViewportDimension(std::uint32_t newWidth, std::uint32_t newHeight);

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
	VkImageView &imageView;
	VkSampler &imageSampler;
	
	std::uint32_t width;
	std::uint32_t height;

	VkDescriptorSetLayout descriptorSetLayout; 
	uint32_t swapChainImageCollectionSize;
	std::vector<std::shared_ptr<VulkanBuffer>> uniformBufferCollection;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSetCollection;

};