#include "VulkanUniform.h"


VulkanUniform::VulkanUniform(VkPhysicalDevice &physicalDevice, VkDevice &logicalDevice, uint32_t swapChainImageCollectionSize, VkExtent2D &swapchainExtent, VkImageView &imageView, VkSampler &imageSampler) :
	logicalDevice(logicalDevice), 
	physicalDevice(physicalDevice), 
	swapChainImageCollectionSize(swapChainImageCollectionSize),
	swapchainExtent(swapchainExtent),
	imageView(imageView),
	imageSampler(imageSampler)
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

void VulkanUniform::updateUniformData(uint32_t currFrameIdx)
{
	if (this->swapchainExtent.height <= 0) { 
		return; 
	}

	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj  = glm::perspective(glm::radians(45.0f), static_cast<float>(this->swapchainExtent.width / this->swapchainExtent.height), 0.1f, 10.0f);

	ubo.proj[1][1] *= -1;

	VkDeviceSize uboSize = sizeof(ubo);
	VulkanUtils::MapMemory(
		this->logicalDevice, 
		this->uniformBufferCollection[currFrameIdx]->getDeviceMemory(), 
		&ubo, 
		uboSize
	);

	/*
	void* data;
	vkMapMemory(this->logicalDevice, this->uniformBufferCollection[currFrameIdx]->getDeviceMemory(), 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(this->logicalDevice, this->uniformBufferCollection[currFrameIdx]->getDeviceMemory());
	*/

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

	// the VulkanBuffer objects are reference counted and destroy them self
	//for (size_t i = 0; i < this->swapChainImageCollectionSize; i++) {
		//this->uniformBufferCollection[i]->freeMemory();
		//delete this->uniformBufferCollection[i];
	//}

}

void VulkanUniform::initDescriptorPool() {


	//VkDescriptorPoolSize poolSize = {};
	//poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//poolSize.descriptorCount = static_cast<uint32_t>(this->swapChainImageCollectionSize);



	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(this->swapChainImageCollectionSize);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(this->swapChainImageCollectionSize);



	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(this->swapChainImageCollectionSize);
	
	VkResult res = vkCreateDescriptorPool(this->logicalDevice, &poolInfo, nullptr, &descriptorPool);
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
	allocInfo.descriptorPool = this->descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(this->swapChainImageCollectionSize);
	allocInfo.pSetLayouts = layouts.data();

	this->descriptorSetCollection.resize(this->swapChainImageCollectionSize);

	VkResult res = vkAllocateDescriptorSets(this->logicalDevice, &allocInfo, this->descriptorSetCollection.data());
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate descriptor sets.");
	}

	for (uint32_t i = 0; i < this->swapChainImageCollectionSize; i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = this->uniformBufferCollection[i]->getBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range  = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView   = this->imageView;
		imageInfo.sampler     = this->imageSampler;
		

		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = this->descriptorSetCollection[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;
		//descriptorWrites[0].pTexelBufferView = nullptr; // Optional

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = this->descriptorSetCollection[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;
		//descriptorWrites[1].pTexelBufferView = nullptr; // Optional

		vkUpdateDescriptorSets(this->logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);


		//VkWriteDescriptorSet descriptorWrite = {};
		//descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//descriptorWrite.dstSet = this->descriptorSetCollection[i];
		//descriptorWrite.dstBinding = 0;
		//descriptorWrite.dstArrayElement = 0;
		//descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//descriptorWrite.descriptorCount = 1;
		//descriptorWrite.pBufferInfo = &bufferInfo;
		//descriptorWrite.pImageInfo = nullptr; // Optional
		//descriptorWrite.pTexelBufferView = nullptr; // Optional

		//vkUpdateDescriptorSets(this->logicalDevice, 1, &descriptorWrite, 0, nullptr);

	}



}
