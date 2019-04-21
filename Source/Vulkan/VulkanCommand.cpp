#include "VulkanCommand.h"

VulkanCommand::VulkanCommand(VkPhysicalDevice &physicalDev, VkDevice &logicalDevice, DeviceInfo &deviceInfo, std::vector<VkFramebuffer> &frameBufferCollection, VkRenderPass &renderpass, VkExtent2D &swapchainExtent, VkPipeline &graphicsPipeline, VkPipelineLayout &pipelineLayout, VkQueue &graphicsQueue, std::vector<VkDescriptorSet> &descriptorSetCollection) :
	physicalDev(physicalDev),
	logicalDevice(logicalDevice),
	deviceInfo(deviceInfo),
	frameBufferCollection(frameBufferCollection),
	renderpass(renderpass),
	swapchainExtent(swapchainExtent),
	graphicsPipeline(graphicsPipeline),
	graphicsQueue(graphicsQueue),
	pipelineLayout(pipelineLayout),
	descriptorSetCollection(descriptorSetCollection)
{
	this->init_commandPool();
}

VulkanCommand::~VulkanCommand() 
{
	vkDestroyCommandPool(this->logicalDevice, this->commandPool, nullptr);
}

std::vector<VkCommandBuffer> &VulkanCommand::GetDrawCommandBufferCollection()
{
	return this->drawCommandBufferCollection;
}

// source buffer is always staging buffer
void VulkanCommand::UploadVertexData(std::vector<VulkanVertex> &vertexData, std::vector<uint16_t> &indexCollection)
{

	// ----------------------------------------------------------
	
	VkDeviceSize vertexBufferSize = sizeof(VulkanVertex) * vertexData.size();
	std::shared_ptr<VulkanBuffer> vertexStagingBufferObj(VulkanBuffer::NewStagingBuffer(this->physicalDev, this->logicalDevice, vertexBufferSize));
	this->vertexBuffer = VulkanBuffer::NewVertexBuffer(this->physicalDev,  this->logicalDevice, vertexBufferSize);
	
	// Copy Vertex data to staging buffer
	{
		VulkanUtils::MapMemory(
			this->logicalDevice, 
			vertexStagingBufferObj->getDeviceMemory(), 
			vertexData.data(), 
			vertexStagingBufferObj->getSize()
		);
	}

	VkDeviceSize indexBufferSize = sizeof(uint16_t) * indexCollection.size();
	std::shared_ptr<VulkanBuffer> indexStagingBufferObj(VulkanBuffer::NewStagingBuffer(this->physicalDev, this->logicalDevice, indexBufferSize));
	this->indexBuffer = VulkanBuffer::NewIndexBuffer(this->physicalDev, this->logicalDevice, indexBufferSize);
	
	// Copy Index data to staging buffer
	{
		VulkanUtils::MapMemory(
			this->logicalDevice,
			indexStagingBufferObj->getDeviceMemory(),
			indexCollection.data(),
			indexStagingBufferObj->getSize()
		);
	}

	// ----------------------------------------------------------

	VkCommandBuffer commandBuffer = this->BeginSingleTimeCommands();

	VkBufferCopy copyRegionVertexBuffer = {};
	copyRegionVertexBuffer.size = vertexStagingBufferObj->getSize();
	vkCmdCopyBuffer(commandBuffer, vertexStagingBufferObj->getBuffer(), this->vertexBuffer->getBuffer(), 1, &copyRegionVertexBuffer);

	VkBufferCopy copyRegionIndexBuffer = {};
	copyRegionIndexBuffer.size = indexStagingBufferObj->getSize();
	vkCmdCopyBuffer(commandBuffer, indexStagingBufferObj->getBuffer(),  this->indexBuffer->getBuffer(), 1, &copyRegionIndexBuffer);

	this->EndSingleTimeCommands(commandBuffer);

	// ----------------------------------------------------------

	// TODO: improve this part
	{
		this->vertexCount = static_cast<uint32_t>(vertexData.size());
		this->indexCount  = static_cast<uint16_t>(indexCollection.size());
	}
	
	this->init_drawCommand();
}

void VulkanCommand::UploadImage(std::shared_ptr<VulkanTexture> &vulkanTexture)
{
    this->imageTexture = vulkanTexture;

	// Upload texture with a staging buffer same procedure as the vertex data
	auto format = vulkanTexture->getQImage()->format();
	if (format != QImage::Format_RGBA8888) {
		throw new std::runtime_error("Format of QImage mismatch, this exception will be removed.");
	}

	VkDeviceSize imageSize = vulkanTexture->getQImage()->byteCount();
	std::shared_ptr<VulkanBuffer> imageStagingBufferObj(VulkanBuffer::NewStagingBuffer(this->physicalDev, this->logicalDevice, imageSize));

	{
		VulkanUtils::MapMemory(
			this->logicalDevice, 
			imageStagingBufferObj->getDeviceMemory(), 
			vulkanTexture->getQImage()->bits(), 
			imageSize
		);
	}

	this->TransitionImageLayout(this->imageTexture->GetImage(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		this->copyBufferToImage(imageStagingBufferObj->getBuffer(), imageTexture->GetImage(), static_cast<uint32_t>(imageTexture->getQImage()->width()), static_cast<uint32_t>(imageTexture->getQImage()->height()));
	this->TransitionImageLayout(this->imageTexture->GetImage(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}

void VulkanCommand::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
	VkCommandBuffer commandBuffer = this->BeginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	this->EndSingleTimeCommands(commandBuffer);
}

void VulkanCommand::init_commandPool()
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = this->deviceInfo.queueFamilyIndexes.graphics;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional
	
	VkResult res = vkCreateCommandPool(this->logicalDevice, &poolInfo, nullptr, &this->commandPool);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool.");
	}
}

void VulkanCommand::init_drawCommand()
{
	VkResult res;

	this->drawCommandBufferCollection.clear();
	this->drawCommandBufferCollection.resize(static_cast<uint32_t>(this->frameBufferCollection.size()));

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = this->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(this->drawCommandBufferCollection.size());

	res = vkAllocateCommandBuffers(this->logicalDevice, &allocInfo, this->drawCommandBufferCollection.data());
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffer.");
	}
	// Starting command buffer recording

	uint32_t idx = 0;
	for (VkFramebuffer framebuffer : this->frameBufferCollection) {

		// Command buffer recording
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		res = vkBeginCommandBuffer(this->drawCommandBufferCollection[idx], &beginInfo);
		if (res != VkResult::VK_SUCCESS) {
			throw std::runtime_error("Failed to begin command buffer.");
		}

		// Starting a render pass
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass  = this->renderpass;
		renderPassInfo.framebuffer = framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = this->swapchainExtent;


		std::array< VkClearValue, 2> clearValues = {};
		clearValues[0].color					 = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil				 = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues    = clearValues.data();


		// Crashed before because of the many swapchain Images allocated in the VulkanSwapchain class
		vkCmdBeginRenderPass(this->drawCommandBufferCollection[idx], &renderPassInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(this->drawCommandBufferCollection[idx], VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicsPipeline);
		
		VkBuffer vertexBuffers[] = { this->vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		
		// this->vertexCount was not set so it had (maximum number) which led to a device lost error
		//vkCmdDraw(this->drawCommandBufferCollection[idx], this->vertexCount, 1, 0, 0);


		vkCmdBindVertexBuffers(this->drawCommandBufferCollection[idx], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(this->drawCommandBufferCollection[idx], this->indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(this->drawCommandBufferCollection[idx], VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayout, 0, 1, &this->descriptorSetCollection[idx], 0, nullptr);
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

VkCommandBuffer VulkanCommand::BeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = this->commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(this->logicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void VulkanCommand::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo         = {};
	submitInfo.sType			    = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext			    = nullptr;
	submitInfo.waitSemaphoreCount   = 0;
	submitInfo.pWaitSemaphores      = nullptr;
	submitInfo.pWaitDstStageMask    = nullptr;
	submitInfo.commandBufferCount   = 1;
	submitInfo.pCommandBuffers	    = &commandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores	= nullptr;

	VkResult res;
	res = vkQueueSubmit(this->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to submit command to transfer queue.");
	}

	res = vkQueueWaitIdle(this->graphicsQueue);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Queue wait idle error.");
	}

	vkFreeCommandBuffers(this->logicalDevice, this->commandPool, 1, &commandBuffer);
}



void VulkanCommand::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = this->BeginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;

	//barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (VulkanUtils::HasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

	}
	else {
		throw std::invalid_argument("Unsupported layout transition.");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	this->EndSingleTimeCommands(commandBuffer);

}
