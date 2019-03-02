#include "VulkanUniform.h"


VulkanUniform::VulkanUniform(VkDevice &logicalDevice) : 
	logicalDevice(logicalDevice) 
{
	this->initDescriptorSetLayout();
}

VulkanUniform::~VulkanUniform() {
	this->destroyDescriptorSetLayout();
}

VkDescriptorSetLayout &VulkanUniform::getDescriptorSetLayout()
{
	return this->descriptorSetLayout;
}

void VulkanUniform::initUniformBuffer() {

}

void VulkanUniform::initDescriptorPool() {

}

void VulkanUniform::initDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding descSetLayoutBinding = {};
	descSetLayoutBinding.binding = 0;
	descSetLayoutBinding.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descSetLayoutBinding.descriptorCount = 1;
	descSetLayoutBinding.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
	descSetLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo descSetLayoutCreateInfo = {};
	descSetLayoutCreateInfo.bindingCount = 1;
	descSetLayoutCreateInfo.flags = 0;
	descSetLayoutCreateInfo.pBindings = &descSetLayoutBinding;
	descSetLayoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutCreateInfo.pNext = nullptr;

	VkResult res = vkCreateDescriptorSetLayout(this->logicalDevice, &descSetLayoutCreateInfo, nullptr, &this->descriptorSetLayout);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}


}

void VulkanUniform::destroyDescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(this->logicalDevice, this->descriptorSetLayout, nullptr);
}
