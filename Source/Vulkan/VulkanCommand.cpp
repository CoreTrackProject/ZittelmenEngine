#include "VulkanCommand.h"


VulkanCommand::VulkanCommand(VkPhysicalDevice &physicalDev, VkDevice &logicalDevice, DeviceInfo &deviceInfo, std::vector<VkFramebuffer> &frameBufferCollection, VkRenderPass &renderpass, VkExtent2D &swapchainExtent, VkPipeline &graphicsPipeline, VkQueue transferQueue) :
	physicalDev(physicalDev),
	logicalDevice(logicalDevice),
	deviceInfo(deviceInfo),
	frameBufferCollection(frameBufferCollection),
	renderpass(renderpass),
	swapchainExtent(swapchainExtent),
	graphicsPipeline(graphicsPipeline),
	transferQueue(transferQueue)
{
	this->init_commandPool();
	//this->init_drawCommand();
}

VulkanCommand::~VulkanCommand() {

	vkFreeMemory(this->logicalDevice,    this->vertexBufferMemory, nullptr);
	vkDestroyBuffer(this->logicalDevice, this->vertexBuffer, nullptr);
	vkDestroyCommandPool(this->logicalDevice, this->commandPool, nullptr);

}

std::vector<VkCommandBuffer>& VulkanCommand::getDrawCommandBufferCollection()
{
	return this->drawCommandBufferCollection;
}

// source buffer is always staging buffer
void VulkanCommand::setVertexData(std::vector<Vertex> &vertexData)
{

	this->vertexCount = vertexData.size();
	// ----------------------------------------------------------
	
	// Create staging buffer here
	// Create vertex buffer
	// Upload vertex buffer

	VkDeviceSize bufferSize = sizeof(vertexData[0]) * vertexData.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VkResult res;

	// Staging buffer creation logic should be more generic and in the vulkan factory class
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size        = bufferSize;
		bufferInfo.usage       = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		res = vkCreateBuffer(this->logicalDevice, &bufferInfo, nullptr, &stagingBuffer);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to create staging buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(this->logicalDevice, stagingBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType			  = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize  = memRequirements.size;
		allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		res = vkAllocateMemory(this->logicalDevice, &allocInfo, nullptr, &stagingBufferMemory);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocated staging buffer memory");
		}

		res = vkBindBufferMemory(this->logicalDevice, stagingBuffer, stagingBufferMemory, 0);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to bind staging buffer memory");
		}
	}

	// Copy Vertex data to staging buffer
	{
		void* data;
		vkMapMemory(this->logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertexData.data(), (size_t)bufferSize);
		vkUnmapMemory(this->logicalDevice, stagingBufferMemory);
	}

	// Vertex buffer creation
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		res = vkCreateBuffer(this->logicalDevice, &bufferInfo, nullptr, &vertexBuffer);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vertex buffer");
		}

		// Buffer memory allocation begins
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(this->logicalDevice, vertexBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		res = vkAllocateMemory(this->logicalDevice, &allocInfo, nullptr, &vertexBufferMemory);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate vertex buffer memory.");
		}

		res = vkBindBufferMemory(this->logicalDevice, vertexBuffer, vertexBufferMemory, 0);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to bind vertex memory buffer");
		}
	}

	// ----------------------------------------------------------

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType				 = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext				 = nullptr;
	commandBufferAllocateInfo.commandBufferCount = 1;
	commandBufferAllocateInfo.commandPool		 = this->commandPool;
	commandBufferAllocateInfo.level				 = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	res = vkAllocateCommandBuffers(this->logicalDevice, &commandBufferAllocateInfo, &commandBuffer);

	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType			= VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext			= nullptr;
	commandBufferBeginInfo.pInheritanceInfo = nullptr; // Pointer to primary commandbuffer
	
	// Record Commands
	{
		res = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.size = bufferSize;
		vkCmdCopyBuffer(commandBuffer, stagingBuffer, this->vertexBuffer, 1, &copyRegion);

		res = vkEndCommandBuffer(commandBuffer);
	}

	// Submit Commands
	{
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;


		// TODO: Need Transfer Queue and probably more here
		res = vkQueueSubmit(this->transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
		res = vkQueueWaitIdle(this->transferQueue);
	}

	vkFreeCommandBuffers(this->logicalDevice, this->commandPool, 1, &commandBuffer);

	// Destroy staging buffer
	vkFreeMemory(this->logicalDevice, stagingBufferMemory, nullptr);
	vkDestroyBuffer(this->logicalDevice, stagingBuffer, nullptr);



	this->init_drawCommand();
}

void VulkanCommand::init_commandPool()
{	
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = this->deviceInfo.queueFamilyIndexes.graphics;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional
	
	VkResult res = vkCreateCommandPool(this->logicalDevice, &poolInfo, nullptr, &this->commandPool);
}

void VulkanCommand::init_drawCommand()
{
	VkResult res;

	this->drawCommandBufferCollection.clear();
	this->drawCommandBufferCollection.resize((uint32_t)this->frameBufferCollection.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = this->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)this->drawCommandBufferCollection.size();

	res = vkAllocateCommandBuffers(this->logicalDevice, &allocInfo, this->drawCommandBufferCollection.data());
	
	// Starting command buffer recording

	uint32_t idx = 0;
	for (VkFramebuffer framebuffer : this->frameBufferCollection) {

		// Command buffer recording
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		res = vkBeginCommandBuffer(this->drawCommandBufferCollection[idx], &beginInfo);

		// Starting a render pass
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass  = this->renderpass;
		renderPassInfo.framebuffer = framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = this->swapchainExtent;

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;


		// Crashed before because of the many swapchain Images allocated in the VulkanSwapchain class
		vkCmdBeginRenderPass(this->drawCommandBufferCollection[idx], &renderPassInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(this->drawCommandBufferCollection[idx], VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicsPipeline);
		

		VkBuffer vertexBuffers[] = { this->vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(this->drawCommandBufferCollection[idx], 0, 1, vertexBuffers, offsets);


		vkCmdDraw(this->drawCommandBufferCollection[idx], this->vertexCount, 1, 0, 0);
		vkCmdEndRenderPass(this->drawCommandBufferCollection[idx]);


		res = vkEndCommandBuffer(this->drawCommandBufferCollection[idx]);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}

		idx++;
	}
	//Starting a render pass

}


// TODO: This method is similar to the one on VulkanFactory
// -> Put this and others to a new class as static method (maybe VulkanDataProvider)
uint32_t VulkanCommand::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(this->physicalDev, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}