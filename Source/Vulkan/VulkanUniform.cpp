#include "VulkanUniform.h"


VulkanUniform::VulkanUniform(VkPhysicalDevice &physicalDevice, VkDevice &logicalDevice, uint32_t swapChainImageCollectionSize) :
	logicalDevice(logicalDevice), 
	physicalDevice(physicalDevice), 
	swapChainImageCollectionSize(swapChainImageCollectionSize)
{
	this->initDescriptorSetLayout();
	this->initUniformBuffer();
	this->initDescriptorPool();
	this->initDescriptorSet();
}

VulkanUniform::~VulkanUniform() {
	vkDestroyDescriptorPool(this->logicalDevice, descriptorPool, nullptr);
	this->destroyUniformBuffer();
	this->destroyDescriptorSetLayout();
}

VkDescriptorSetLayout &VulkanUniform::getDescriptorSetLayout()
{
	return this->descriptorSetLayout;
}

std::vector<VkDescriptorSet> &VulkanUniform::getDescriptorSetCollection()
{
	return this->descriptorSetCollection;
}

void VulkanUniform::initUniformBuffer() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	//this->uniformBufferCollection.resize(this->swapChainImageCollectionSize);
	
	for (size_t i = 0; i < this->swapChainImageCollectionSize; i++) {
		this->uniformBufferCollection.push_back(VulkanBuffer::newUniformBuffer(this->physicalDevice, this->logicalDevice, bufferSize));
	}
}

void VulkanUniform::destroyUniformBuffer()
{
	for (size_t i = 0; i < this->swapChainImageCollectionSize; i++) {
		//this->uniformBufferCollection[i]->freeMemory();
		delete this->uniformBufferCollection[i];
	}

}

void VulkanUniform::initDescriptorPool() {
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(this->swapChainImageCollectionSize);


	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(this->swapChainImageCollectionSize);

	if (vkCreateDescriptorPool(this->logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor pool.");
	}

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

void VulkanUniform::initDescriptorSet()
{
	std::vector<VkDescriptorSetLayout> layouts(this->swapChainImageCollectionSize, this->descriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(this->swapChainImageCollectionSize);
	allocInfo.pSetLayouts = layouts.data();

	this->descriptorSetCollection.resize(this->swapChainImageCollectionSize);
	if (vkAllocateDescriptorSets(this->logicalDevice, &allocInfo, this->descriptorSetCollection.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate descriptor sets.");
	}

	for (uint32_t i = 0; i < this->swapChainImageCollectionSize; i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = this->uniformBufferCollection[i]->getBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = this->descriptorSetCollection[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional

		vkUpdateDescriptorSets(this->logicalDevice, 1, &descriptorWrite, 0, nullptr);

	}



}
