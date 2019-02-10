#include "VulkanCommand.h"


VulkanCommand::VulkanCommand(VkPhysicalDevice &physicalDev, VkDevice &logicalDevice, DeviceInfo &deviceInfo, std::vector<VkFramebuffer> &frameBufferCollection, VkRenderPass &renderpass, VkExtent2D &swapchainExtent, VkPipeline &graphicsPipeline, VkBuffer &vertexBuffer, uint32_t vertexCount, VkQueue transferQueue) :
	physicalDev(physicalDev),
	logicalDevice(logicalDevice),
	deviceInfo(deviceInfo),
	frameBufferCollection(frameBufferCollection),
	renderpass(renderpass),
	swapchainExtent(swapchainExtent),
	graphicsPipeline(graphicsPipeline),
	vertexBuffer(vertexBuffer),
	vertexCount(vertexCount),
	transferQueue(transferQueue)
{
	this->init_commandPool();
	this->init_commandBuffer();
}

VulkanCommand::~VulkanCommand() {
	vkDestroyCommandPool(this->logicalDevice, this->commandPool, nullptr);
}

std::vector<VkCommandBuffer>& VulkanCommand::getCommandBufferCollection()
{
	return this->commandBufferCollection;
}


// source buffer is always staging buffer
void VulkanCommand::uploadBuffer(VkBuffer &destinationBuffer, VkDeviceSize bufferSize)
{
	// ----------------------------------------------------------
	
	// Create stagin buffer here

	VkBuffer	   stagingBuffer	   = VK_NULL_HANDLE;
	VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

	// Staging buffer creation logic should be more generic and in the vulkan factory class
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size        = bufferSize;
		bufferInfo.usage       = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(this->logicalDevice, &bufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create staging buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(this->logicalDevice, stagingBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType			  = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize  = memRequirements.size;
		allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


		if (vkAllocateMemory(this->logicalDevice, &allocInfo, nullptr, &stagingBufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(this->logicalDevice, stagingBuffer, stagingBufferMemory, 0);
	}

	// ----------------------------------------------------------


	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType				 = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext				 = nullptr;
	commandBufferAllocateInfo.commandBufferCount = 1;
	commandBufferAllocateInfo.commandPool		 = this->commandPool;
	commandBufferAllocateInfo.level				 = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	vkAllocateCommandBuffers(this->logicalDevice, &commandBufferAllocateInfo, &commandBuffer);

	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType			= VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext			= nullptr;
	commandBufferBeginInfo.pInheritanceInfo = nullptr; // Pointer to primary commandbuffer
	
	// Record Commands
	{
		vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.size = bufferSize;
		vkCmdCopyBuffer(commandBuffer, stagingBuffer, destinationBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);
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
		vkQueueSubmit(this->transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(this->transferQueue);

	}

	vkFreeCommandBuffers(this->logicalDevice, this->commandPool, 1, &commandBuffer);
	vkFreeMemory(this->logicalDevice, stagingBufferMemory, nullptr);
	vkDestroyBuffer(this->logicalDevice, stagingBuffer, nullptr);

}

void VulkanCommand::init_commandPool()
{	
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = this->deviceInfo.queueFamilyIndexes.graphics;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional
	
	VkResult res = vkCreateCommandPool(this->logicalDevice, &poolInfo, nullptr, &this->commandPool);
}

void VulkanCommand::init_commandBuffer()
{
	this->commandBufferCollection.resize((uint32_t)this->frameBufferCollection.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = this->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)this->commandBufferCollection.size();

	VkResult res = vkAllocateCommandBuffers(this->logicalDevice, &allocInfo, this->commandBufferCollection.data());
	
	// Starting command buffer recording

	uint32_t idx = 0;
	for (VkFramebuffer framebuffer : this->frameBufferCollection) {

		// Command buffer recording
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		VkResult res = vkBeginCommandBuffer(this->commandBufferCollection[idx], &beginInfo);

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
		vkCmdBeginRenderPass(this->commandBufferCollection[idx], &renderPassInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(this->commandBufferCollection[idx], VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicsPipeline);
		

		VkBuffer vertexBuffers[] = { this->vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(this->commandBufferCollection[idx], 0, 1, vertexBuffers, offsets);


		vkCmdDraw(this->commandBufferCollection[idx], this->vertexCount, 1, 0, 0);


		vkCmdEndRenderPass(this->commandBufferCollection[idx]);

		//res = vkEndCommandBuffer(this->commandBufferCollection[idx]);
		if (vkEndCommandBuffer(this->commandBufferCollection[idx]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
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