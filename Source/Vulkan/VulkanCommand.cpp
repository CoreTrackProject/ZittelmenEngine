#include "VulkanCommand.h"


VulkanCommand::VulkanCommand(VkDevice &logicalDevice, DeviceInfo &deviceInfo, std::vector<VkFramebuffer> &frameBufferCollection, VkRenderPass &renderpass, VkExtent2D &swapchainExtent, VkPipeline &graphicsPipeline)
	: logicalDevice(logicalDevice), deviceInfo(deviceInfo), frameBufferCollection(frameBufferCollection), renderpass(renderpass), swapchainExtent(swapchainExtent), graphicsPipeline(graphicsPipeline)
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
		vkCmdDraw(this->commandBufferCollection[idx], 3, 1, 0, 0);
		vkCmdEndRenderPass(this->commandBufferCollection[idx]);

		//res = vkEndCommandBuffer(this->commandBufferCollection[idx]);
		if (vkEndCommandBuffer(this->commandBufferCollection[idx]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		idx++;
	}
	//Starting a render pass

}
