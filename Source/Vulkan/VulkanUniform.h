#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <stdexcept>

// https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class VulkanUniform {

public:
	VulkanUniform(VkDevice &logicalDevice);
	~VulkanUniform();

	VkDescriptorSetLayout &getDescriptorSetLayout();

private:
	void initUniformBuffer();
	void initDescriptorPool();

	void initDescriptorSetLayout();
	void destroyDescriptorSetLayout();


private:
	VkDevice &logicalDevice;
	VkDescriptorSetLayout descriptorSetLayout;

};