#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>

/*

*/
struct VulkanVertex {
    glm::vec3 pos;
    glm::vec3 color;
	glm::vec2 texCoord;
};

/*

*/
class VulkanVertexData {

public:
	VulkanVertexData()  {};
	~VulkanVertexData() {};

	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};

		bindingDescription.binding   = 0;
		bindingDescription.stride    = sizeof(VulkanVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		attributeDescriptions[0].binding  = 0;
		attributeDescriptions[0].location = 0; // Location in shader
		attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset   = offsetof(VulkanVertex, pos);


		attributeDescriptions[1].binding  = 0;
		attributeDescriptions[1].location = 1; // Location in shader
		attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset   = offsetof(VulkanVertex, color);

		attributeDescriptions[2].binding  = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset   = offsetof(VulkanVertex, texCoord);

		return attributeDescriptions;
	}

};