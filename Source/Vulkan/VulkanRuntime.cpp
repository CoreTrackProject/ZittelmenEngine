#include "VulkanRuntime.h"


VulkanRuntime::VulkanRuntime(VulkanRuntimeCreateInfo createInfo) : createInfo(createInfo) {

	this->init_syncobjects();
}

VulkanRuntime::~VulkanRuntime() {
	this->destroy();
}


void VulkanRuntime::RenderFrame()
{
	if (this->renderingFailed) {
		return;
	}

	

	VkResult res = vkWaitForFences(this->createInfo.logicalDevice, 1, &this->inFlightFences[this->currentFrameIdx], VK_FALSE, 1000000000);
	if (res != VkResult::VK_SUCCESS) {
		
		this->renderingFailed = true;
		// TODO: Restart renderer

		return;
	}

	res =  vkResetFences(this->createInfo.logicalDevice, 1, &this->inFlightFences[this->currentFrameIdx]);
	if (res != VkResult::VK_SUCCESS) {
		this->renderingFailed = true;
		//throw std::runtime_error("Failed to reset fences.");
	}

	std::uint32_t imageIndex = 0;
	res = vkAcquireNextImageKHR(
		this->createInfo.logicalDevice,
		this->createInfo.swapchain,
		std::numeric_limits<uint64_t>::max(),
		this->imageAvailableSemaphoreCollection[this->currentFrameIdx],
		VK_NULL_HANDLE,
		&imageIndex);

	if (res != VkResult::VK_SUCCESS) {
		this->renderingFailed = true;
		//throw std::runtime_error("Failed acquire next image.");
	}

	this->createInfo.uniform->updateUniformData(imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { this->imageAvailableSemaphoreCollection[this->currentFrameIdx] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores    = waitSemaphores;
	submitInfo.pWaitDstStageMask  = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &this->createInfo.commandBufferCollection[imageIndex];

	VkSemaphore signalSemaphores[] = { this->renderFinishedSemaphoreCollection[this->currentFrameIdx] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	res = vkQueueSubmit(this->createInfo.graphicsQueue, 1, &submitInfo, this->inFlightFences[currentFrameIdx]);
	if (res != VK_SUCCESS) {
		this->renderingFailed = true;
		//throw std::runtime_error("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { this->createInfo.swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	res = vkQueuePresentKHR(this->createInfo.presentQueue, &presentInfo);
	if (res != VK_SUCCESS) {
		this->renderingFailed = true;
		//throw std::runtime_error("Failed to submit to present queue.");
	}

	this->currentFrameIdx = (this->currentFrameIdx + 1) % MAX_FRAMES_IN_FLIGHT;
}

size_t VulkanRuntime::getCurrentFrameIdx()
{
	return this->currentFrameIdx;
}

void VulkanRuntime::destroy()
{
	vkDeviceWaitIdle(this->createInfo.logicalDevice);
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(this->createInfo.logicalDevice, renderFinishedSemaphoreCollection[i], nullptr);
		vkDestroySemaphore(this->createInfo.logicalDevice, imageAvailableSemaphoreCollection[i], nullptr);
		vkDestroyFence(this->createInfo.logicalDevice,     inFlightFences[i], nullptr);
	}
	
}

void VulkanRuntime::init_syncobjects()
{
	this->imageAvailableSemaphoreCollection.resize(MAX_FRAMES_IN_FLIGHT);
	this->renderFinishedSemaphoreCollection.resize(MAX_FRAMES_IN_FLIGHT);
	this->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(this->createInfo.logicalDevice, &semaphoreInfo, nullptr, &this->imageAvailableSemaphoreCollection[i]) != VK_SUCCESS ||
			vkCreateSemaphore(this->createInfo.logicalDevice, &semaphoreInfo, nullptr, &this->renderFinishedSemaphoreCollection[i]) != VK_SUCCESS ||
			vkCreateFence(this->createInfo.logicalDevice,     &fenceInfo,     nullptr, &this->inFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create semaphores for a frame!");
		}
	}
}
