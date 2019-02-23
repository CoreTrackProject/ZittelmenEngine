#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

// https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class VulkanUniform {

public:
	VulkanUniform();
	~VulkanUniform();

private:
	void initUniformBuffer();
	void initDescriptorPool();

};