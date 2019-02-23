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
}

VulkanCommand::~VulkanCommand() {

	vkFreeMemory(this->logicalDevice,		  this->indexBufferMemory, nullptr);
	vkDestroyBuffer(this->logicalDevice,	  this->indexBuffer, nullptr);

	vkFreeMemory(this->logicalDevice,		  this->vertexBufferMemory, nullptr);
	vkDestroyBuffer(this->logicalDevice,	  this->vertexBuffer, nullptr);

	vkDestroyCommandPool(this->logicalDevice, this->commandPool, nullptr);
}

std::vector<VkCommandBuffer> &VulkanCommand::getDrawCommandBufferCollection()
{
	return this->drawCommandBufferCollection;
}

// source buffer is always staging buffer
void VulkanCommand::uploadVertexData(std::vector<Vertex> &vertexData, std::vector<uint16_t> &indexCollection)
{

	this->vertexCount = vertexData.size();
	this->indexCount  = indexCollection.size();

	// ----------------------------------------------------------
	
	// Create staging buffer here
	// Create vertex buffer
	// Upload vertex buffer

	VkDeviceSize vertexBufferSize = sizeof(vertexData[0]) * vertexData.size();
	VkBuffer vertexStagingBuffer;
	VkDeviceMemory vertexStagingBufferMemory;


	VkDeviceSize indexBufferSize = sizeof(indexCollection[0]) * indexCollection.size();
	VkBuffer indexStagingBuffer;
	VkDeviceMemory indexStagingBufferMemory;
	

	VkResult res;

	// TODO: Export this code to VulkanFactory

	// Staging buffer creation logic should be more generic and in the vulkan factory class
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size        = vertexBufferSize;
		bufferInfo.usage       = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		res = vkCreateBuffer(this->logicalDevice, &bufferInfo, nullptr, &vertexStagingBuffer);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to create staging buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(this->logicalDevice, vertexStagingBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType			  = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize  = memRequirements.size;
		allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		res = vkAllocateMemory(this->logicalDevice, &allocInfo, nullptr, &vertexStagingBufferMemory);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocated staging buffer memory");
		}

		res = vkBindBufferMemory(this->logicalDevice, vertexStagingBuffer, vertexStagingBufferMemory, 0);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to bind staging buffer memory");
		}
	}

	// Copy Vertex data to staging buffer
	{
		void* data;
		vkMapMemory(this->logicalDevice, vertexStagingBufferMemory, 0, vertexBufferSize, 0, &data);
		memcpy(data, vertexData.data(), (size_t)vertexBufferSize);
		vkUnmapMemory(this->logicalDevice, vertexStagingBufferMemory);
	}

	// Vertex buffer creation
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = vertexBufferSize;
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



	// Index staging buffer creation
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = indexBufferSize;
		bufferInfo.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		res = vkCreateBuffer(this->logicalDevice, &bufferInfo, nullptr, &indexStagingBuffer);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to create index staging buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(this->logicalDevice, indexStagingBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		res = vkAllocateMemory(this->logicalDevice, &allocInfo, nullptr, &indexStagingBufferMemory);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocated index staging buffer memory");
		}

		res = vkBindBufferMemory(this->logicalDevice, indexStagingBuffer, indexStagingBufferMemory, 0);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to bind index staging buffer memory");
		}
	}

	// Copy Index data to staging buffer
	{
		void* data;
		vkMapMemory(this->logicalDevice, indexStagingBufferMemory, 0, indexBufferSize, 0, &data);
		memcpy(data, indexCollection.data(), (size_t)indexBufferSize);
		vkUnmapMemory(this->logicalDevice, indexStagingBufferMemory);
	}

	// Index buffer creation
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = indexBufferSize;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		res = vkCreateBuffer(this->logicalDevice, &bufferInfo, nullptr, &indexBuffer);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to create index buffer");
		}

		// Buffer memory allocation begins
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(this->logicalDevice, indexBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		res = vkAllocateMemory(this->logicalDevice, &allocInfo, nullptr, &indexBufferMemory);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate index buffer memory.");
		}

		res = vkBindBufferMemory(this->logicalDevice, indexBuffer, indexBufferMemory, 0);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to bind index memory buffer");
		}
	}

	// ----------------------------------------------------------

	// Create new command to upload the index data
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

	// Record Commands to upload the vertex data to the GPU
	{
		res = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		VkBufferCopy copyRegionVertexBuffer = {};
		copyRegionVertexBuffer.size = vertexBufferSize;
		vkCmdCopyBuffer(commandBuffer, vertexStagingBuffer, this->vertexBuffer, 1, &copyRegionVertexBuffer);

		VkBufferCopy copyRegionIndexBuffer = {};
		copyRegionIndexBuffer.size  = indexBufferSize;
		vkCmdCopyBuffer(commandBuffer, indexStagingBuffer, this->indexBuffer, 1,  &copyRegionIndexBuffer);

		res = vkEndCommandBuffer(commandBuffer);
	}

	// Submit Commands
	{
		VkSubmitInfo submitInfo = {};
		submitInfo.sType			    = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext			    = nullptr;
		submitInfo.waitSemaphoreCount   = 0;
		submitInfo.pWaitSemaphores      = nullptr;
		submitInfo.pWaitDstStageMask    = nullptr;
		submitInfo.commandBufferCount   = 1;
		submitInfo.pCommandBuffers	    = &commandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores	= nullptr;

		// Upload data with the transfer queue
		res = vkQueueSubmit(this->transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
		res = vkQueueWaitIdle(this->transferQueue);
	}


	vkFreeCommandBuffers(this->logicalDevice, this->commandPool, 1, &commandBuffer);


	// Destroy staging buffer
	vkFreeMemory(this->logicalDevice, vertexStagingBufferMemory, nullptr);
	vkDestroyBuffer(this->logicalDevice, vertexStagingBuffer, nullptr);


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

		vkCmdBindIndexBuffer(this->drawCommandBufferCollection[idx], indexBuffer, 0, VK_INDEX_TYPE_UINT16);


		// this->vertexCount was not set so it had (maximum number) which led to a device lost error
		//vkCmdDraw(this->drawCommandBufferCollection[idx], this->vertexCount, 1, 0, 0);
		
		vkCmdDrawIndexed(this->drawCommandBufferCollection[idx], this->indexCount, 1, 0, 0, 0);


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