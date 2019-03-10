#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>

struct VulkanVertex {
    glm::vec2 pos;
    glm::vec3 color;
};

class VulkanVertexData {

public:
	VulkanVertexData() {};
	~VulkanVertexData() {};




	std::vector<VulkanVertex> getTriangleVertexCollection() {
		std::vector<VulkanVertex> vertices = {
			{
				{0.0f, -0.5f}, {0.0f, 0.0f, 1.0f}
			},
			{
				{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}
			},
			{
				{-0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}
			}
		};
		return vertices;
	};

	std::vector<VulkanVertex> getQuadVertexCollection() {
		std::vector<VulkanVertex> vertices = {
			{
				{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}
			},
			{
				{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}
			},
			{
				{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}
			},
			{
				{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}
			}
		};
		return vertices;
	};

	std::vector<uint16_t> getQuadVertexIndexCollection() {
		std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0
		};
		return indices;
	};

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(VulkanVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

		attributeDescriptions[0].binding  = 0;
		attributeDescriptions[0].location = 0; // Location in shader
		attributeDescriptions[0].format   = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset   = offsetof(VulkanVertex, pos);


		attributeDescriptions[1].binding  = 0;
		attributeDescriptions[1].location = 1; // Location in shader
		attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset   = offsetof(VulkanVertex, color);

		return attributeDescriptions;
	}

};