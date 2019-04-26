#include "VulkanUniform.h"


VulkanUniform::VulkanUniform(VulkanUniformCreateInfo createInfo) : createInfo(createInfo)
{
	this->initDescriptorSetLayout();
	this->initUniformBuffer();
	this->initDescriptorPool();
	this->initDescriptorSet();
}

VulkanUniform::~VulkanUniform() {
	
	vkDestroyDescriptorPool(this->createInfo.logicalDevice, descriptorPool, nullptr);

	this->destroyUniformBuffer();
	this->destroyDescriptorSetLayout();
}

VkDescriptorSetLayout &VulkanUniform::GetDescriptorSetLayout()
{
	return this->descriptorSetLayout;
}

std::vector<VkDescriptorSet> &VulkanUniform::GetDescriptorSetCollection()
{
	return this->descriptorSetCollection;
}

void VulkanUniform::updateUniformData(uint32_t currFrameIdx)
{

	if (this->createInfo.height <= 0) {
		return; 
	}

	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj  = glm::perspective(glm::radians(45.0f), static_cast<float>(this->createInfo.width / this->createInfo.height), 0.1f, 10.0f);

	ubo.proj[1][1] *= -1;

	VkDeviceSize uboSize = sizeof(ubo);
	VulkanUtils::MapMemory(
		this->createInfo.logicalDevice,
		this->uniformBufferCollection[currFrameIdx]->getDeviceMemory(), 
		&ubo, 
		uboSize
	);
}

void VulkanUniform::UpdateViewportDimension(std::uint32_t newWidth, std::uint32_t newHeight)
{
	this->createInfo.width = newWidth;
	this->createInfo.height = newHeight;
}

void VulkanUniform::initUniformBuffer() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	//this->uniformBufferCollection.resize(this->swapChainImageCollectionSize);
	
	for (size_t i = 0; i < this->createInfo.swapChainImageCollectionSize; i++) {
		this->uniformBufferCollection.push_back(VulkanBuffer::NewUniformBuffer(this->createInfo.physicalDevice, this->createInfo.logicalDevice, bufferSize));
	}
}

void VulkanUniform::destroyUniformBuffer()
{
	// the VulkanBuffer objects are reference counted and destroy them self
	//for (size_t i = 0; i < this->swapChainImageCollectionSize; i++) {
		//this->uniformBufferCollection[i]->freeMemory();
		//delete this->uniformBufferCollection[i];
	//}
}

void VulkanUniform::initDescriptorPool() 
{
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(this->createInfo.swapChainImageCollectionSize);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(this->createInfo.swapChainImageCollectionSize);

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(this->createInfo.swapChainImageCollectionSize);
	
	VkResult res = vkCreateDescriptorPool(this->createInfo.logicalDevice, &poolInfo, nullptr, &descriptorPool);
	if (res != VK_SUCCESS) {
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

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	
	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { descSetLayoutBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo descSetLayoutCreateInfo = {};
	descSetLayoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutCreateInfo.flags = 0;
	descSetLayoutCreateInfo.pNext = nullptr;
	descSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
	descSetLayoutCreateInfo.pBindings = bindings.data();
	
	VkResult res = vkCreateDescriptorSetLayout(this->createInfo.logicalDevice, &descSetLayoutCreateInfo, nullptr, &this->descriptorSetLayout);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void VulkanUniform::destroyDescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(this->createInfo.logicalDevice, this->descriptorSetLayout, nullptr);
}

void VulkanUniform::initDescriptorSet()
{
	std::vector<VkDescriptorSetLayout> layouts(this->createInfo.swapChainImageCollectionSize, this->descriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = this->descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(this->createInfo.swapChainImageCollectionSize);
	allocInfo.pSetLayouts = layouts.data();

	this->descriptorSetCollection.resize(this->createInfo.swapChainImageCollectionSize);

	VkResult res = vkAllocateDescriptorSets(this->createInfo.logicalDevice, &allocInfo, this->descriptorSetCollection.data());
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate descriptor sets.");
	}

	for (uint32_t i = 0; i < this->createInfo.swapChainImageCollectionSize; i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = this->uniformBufferCollection[i]->getBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range  = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView   = this->createInfo.imageView;
		imageInfo.sampler     = this->createInfo.imageSampler;
		
		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = this->descriptorSetCollection[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;
		descriptorWrites[0].pTexelBufferView = nullptr; // Optional

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = this->descriptorSetCollection[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;
		descriptorWrites[1].pTexelBufferView = nullptr; // Optional

		vkUpdateDescriptorSets(this->createInfo.logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}
