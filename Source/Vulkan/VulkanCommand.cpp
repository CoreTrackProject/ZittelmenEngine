#include "VulkanCommand.h"

VulkanCommand::VulkanCommand(VulkanCommandCreateInfo createInfo) : createInfo(createInfo)
{
	this->init_commandPool();
}

VulkanCommand::~VulkanCommand() 
{
	vkDestroyCommandPool(this->createInfo.logicalDevice, this->commandPool, nullptr);
}



std::vector<VkCommandBuffer> VulkanCommand::GetDrawCommandBufferCollection()
{

	std::vector<VkCommandBuffer> drawCommandBufferCollection;
	VkResult res;

	drawCommandBufferCollection.resize(static_cast<std::uint32_t>(this->createInfo.frameBufferCollection.size()));

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = this->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<std::uint32_t>(drawCommandBufferCollection.size());

	res = vkAllocateCommandBuffers(this->createInfo.logicalDevice, &allocInfo, drawCommandBufferCollection.data());
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffer.");
	}
	// Starting command buffer recording

	std::uint32_t idx = 0;
	for (VkFramebuffer framebuffer : this->createInfo.frameBufferCollection) {

		// Command buffer recording
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		res = vkBeginCommandBuffer(drawCommandBufferCollection[idx], &beginInfo);
		if (res != VkResult::VK_SUCCESS) {
			throw std::runtime_error("Failed to begin command buffer.");
		}

		// Starting a render pass
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = this->createInfo.renderpass;
		renderPassInfo.framebuffer = framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = this->createInfo.swapchainExtent;

		std::array< VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<std::uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();


		// Crashed before because of the many swapchain Images allocated in the VulkanSwapchain class
		vkCmdBeginRenderPass(drawCommandBufferCollection[idx], &renderPassInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(drawCommandBufferCollection[idx], VK_PIPELINE_BIND_POINT_GRAPHICS, this->createInfo.graphicsPipeline);

		// Dynamic states
		{
			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(this->createInfo.swapchainExtent.width);
			viewport.height = static_cast<float>(this->createInfo.swapchainExtent.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(drawCommandBufferCollection[idx], 0, 1, &viewport);


			VkRect2D rect2D = {};
			rect2D.extent.width = static_cast<float>(this->createInfo.swapchainExtent.width);
			rect2D.extent.height = static_cast<float>(this->createInfo.swapchainExtent.height);
			rect2D.offset.x = 0;
			rect2D.offset.y = 0;
			vkCmdSetScissor(drawCommandBufferCollection[idx], 0, 1, &rect2D);
		}

		// Vertex drawing
		{
			VkBuffer vertexBuffers[] = { this->vertexBuffer->getBuffer() };
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(drawCommandBufferCollection[idx],  0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(drawCommandBufferCollection[idx],    this->indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
			vkCmdBindDescriptorSets(drawCommandBufferCollection[idx], VK_PIPELINE_BIND_POINT_GRAPHICS, this->createInfo.pipelineLayout, 0, 1, &this->createInfo.descriptorSetCollection[idx], 0, nullptr);
			vkCmdDrawIndexed(drawCommandBufferCollection[idx],        this->indexCount, 1, 0, 0, 0);
			
		}

		vkCmdEndRenderPass(drawCommandBufferCollection[idx]);


		res = vkEndCommandBuffer(drawCommandBufferCollection[idx]);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}

		idx++;
	}

	return drawCommandBufferCollection;
}

// source buffer is always staging buffer
void VulkanCommand::UploadVertexData(std::vector<VulkanVertex> &vertexData, std::vector<std::uint32_t> &indexCollection)
{

	// ----------------------------------------------------------
	
	VkDeviceSize vertexBufferSize = sizeof(VulkanVertex) * vertexData.size();
	std::shared_ptr<VulkanBuffer> vertexStagingBufferObj(VulkanBuffer::NewStagingBuffer(this->createInfo.physicalDev, this->createInfo.logicalDevice, vertexBufferSize));
	this->vertexBuffer = VulkanBuffer::NewVertexBuffer(this->createInfo.physicalDev,  this->createInfo.logicalDevice, vertexBufferSize);
	
	// Copy Vertex data to staging buffer
	{
		VulkanUtils::MapMemory(
			this->createInfo.logicalDevice,
			vertexStagingBufferObj->getDeviceMemory(), 
			vertexData.data(), 
			vertexStagingBufferObj->getSize()
		);
	}

	VkDeviceSize indexBufferSize = sizeof(std::uint32_t) * indexCollection.size();
	std::shared_ptr<VulkanBuffer> indexStagingBufferObj(VulkanBuffer::NewStagingBuffer(this->createInfo.physicalDev, this->createInfo.logicalDevice, indexBufferSize));
	this->indexBuffer = VulkanBuffer::NewIndexBuffer(this->createInfo.physicalDev, this->createInfo.logicalDevice, indexBufferSize);
	
	// Copy Index data to staging buffer
	{
		VulkanUtils::MapMemory(
			this->createInfo.logicalDevice,
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
		this->vertexCount = static_cast<std::uint32_t>(vertexData.size());
		this->indexCount  = static_cast<std::uint32_t>(indexCollection.size());
	}
	
	
}

void VulkanCommand::UploadImage(std::shared_ptr<VulkanTexture> &vulkanTexture)
{
    this->imageTexture = vulkanTexture;

	// Upload texture with a staging buffer same procedure as the vertex data
	auto format = vulkanTexture->GetQImage()->format();
	if (format != QImage::Format_RGBA8888) {
		throw new std::runtime_error("Format of QImage mismatch, this exception will be removed.");
	}

	VkDeviceSize imageSize = vulkanTexture->GetQImage()->byteCount();
	std::shared_ptr<VulkanBuffer> imageStagingBufferObj(VulkanBuffer::NewStagingBuffer(this->createInfo.physicalDev, this->createInfo.logicalDevice, imageSize));

	{
		VulkanUtils::MapMemory(
			this->createInfo.logicalDevice,
			imageStagingBufferObj->getDeviceMemory(), 
			vulkanTexture->GetQImage()->bits(), 
			imageSize
		);
	}

	this->TransitionImageLayout(this->imageTexture->GetImage(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	this->copyBufferToImage(imageStagingBufferObj->getBuffer(), imageTexture->GetImage(), static_cast<std::uint32_t>(imageTexture->GetQImage()->width()), static_cast<std::uint32_t>(imageTexture->GetQImage()->height()));
	this->TransitionImageLayout(this->imageTexture->GetImage(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}

VkCommandBuffer VulkanCommand::BeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = this->commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(this->createInfo.logicalDevice, &allocInfo, &commandBuffer);

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
	submitInfo.sType                = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext                = nullptr;
	submitInfo.waitSemaphoreCount   = 0;
	submitInfo.pWaitSemaphores      = nullptr;
	submitInfo.pWaitDstStageMask    = nullptr;
	submitInfo.commandBufferCount   = 1;
	submitInfo.pCommandBuffers      = &commandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores    = nullptr;

	VkResult res;
	res = vkQueueSubmit(this->createInfo.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to submit command to transfer queue.");
	}

	res = vkQueueWaitIdle(this->createInfo.graphicsQueue);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Queue wait idle error.");
	}

	vkFreeCommandBuffers(this->createInfo.logicalDevice, this->commandPool, 1, &commandBuffer);
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
	} else {
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

	} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	} else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

	} else {
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

void VulkanCommand::UpdateFramebufferCollection(std::vector<VkFramebuffer> frameBufferCollection, VkExtent2D newSwapchainExtent)
{

	this->createInfo.frameBufferCollection = frameBufferCollection;
	this->createInfo.swapchainExtent       = newSwapchainExtent;

}

void VulkanCommand::UpdateRenderpass(VkRenderPass renderpass)
{
	this->createInfo.renderpass = renderpass;
}

void VulkanCommand::copyBufferToImage(VkBuffer buffer, VkImage image, std::uint32_t width, std::uint32_t height) {
	VkCommandBuffer commandBuffer = this->BeginSingleTimeCommands();

	VkBufferImageCopy region               = {};
	region.bufferOffset                    = 0;
	region.bufferRowLength                 = 0;
	region.bufferImageHeight               = 0;
	region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel       = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount     = 1;
	region.imageOffset                     = { 0, 0, 0 };
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
	poolInfo.queueFamilyIndex = this->createInfo.deviceInfo.queueFamilyIndexes.graphics;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional
	
	VkResult res = vkCreateCommandPool(this->createInfo.logicalDevice, &poolInfo, nullptr, &this->commandPool);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool.");
	}
}