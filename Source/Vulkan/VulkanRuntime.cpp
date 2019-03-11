#include "VulkanRuntime.h"

VulkanRuntime::VulkanRuntime(VkDevice &logicalDevice, VkSwapchainKHR &swapchain, std::vector<VkCommandBuffer> &commandBufferCollection, VkQueue &graphicsQueue, VkQueue &presentQueue) :
	logicalDevice(logicalDevice),
	swapchain(swapchain),
	commandBufferCollection(commandBufferCollection),
	graphicsQueue(graphicsQueue),
	presentQueue(presentQueue) {

	this->init_syncobjects();
}

VulkanRuntime::~VulkanRuntime() {
	this->destroy();
}

void VulkanRuntime::renderFrame()
{
	if (!this->isRenderActive) {
		return;
	}

	VkResult res = vkWaitForFences(this->logicalDevice, 1, &this->inFlightFences[this->currentFrameIdx], VK_FALSE, 1000000000);
	if (res != VkResult::VK_SUCCESS) {

		// TODO: Restart renderer

		return;
	}

	res =  vkResetFences(this->logicalDevice, 1, &this->inFlightFences[this->currentFrameIdx]);

	uint32_t imageIndex = 0;
	res = vkAcquireNextImageKHR(
		this->logicalDevice,
		this->swapchain,
		std::numeric_limits<uint64_t>::max(),
		this->imageAvailableSemaphoreCollection[this->currentFrameIdx],
		VK_NULL_HANDLE,
		&imageIndex);


	// Update uniform buffers here with imageIndex var
	if (this->updateUBO != nullptr) {
		this->updateUBO(imageIndex);
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { this->imageAvailableSemaphoreCollection[this->currentFrameIdx] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores    = waitSemaphores;
	submitInfo.pWaitDstStageMask  = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &this->commandBufferCollection[imageIndex];

	VkSemaphore signalSemaphores[] = { this->renderFinishedSemaphoreCollection[this->currentFrameIdx] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	res = vkQueueSubmit(this->graphicsQueue, 1, &submitInfo, this->inFlightFences[currentFrameIdx]);
	if (res != VK_SUCCESS) {
		qDebug("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { this->swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	res = vkQueuePresentKHR(this->presentQueue, &presentInfo);
	if (res != VK_SUCCESS) {
		qDebug("failed to submit to present queu!");
	}

	this->currentFrameIdx = (this->currentFrameIdx + 1) % MAX_FRAMES_IN_FLIGHT;
}

size_t VulkanRuntime::getCurrentFrameIdx()
{
	return this->currentFrameIdx;
}

void VulkanRuntime::registerUpdateUBOCallback(std::function<void(uint32_t)> updateUBO)
{
	this->updateUBO = updateUBO;
}

void VulkanRuntime::destroy()
{
	vkDeviceWaitIdle(this->logicalDevice);
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(this->logicalDevice, renderFinishedSemaphoreCollection[i], nullptr);
		vkDestroySemaphore(this->logicalDevice, imageAvailableSemaphoreCollection[i], nullptr);
		vkDestroyFence(this->logicalDevice,     inFlightFences[i], nullptr);
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
		if (vkCreateSemaphore(this->logicalDevice, &semaphoreInfo, nullptr, &this->imageAvailableSemaphoreCollection[i]) != VK_SUCCESS ||
			vkCreateSemaphore(this->logicalDevice, &semaphoreInfo, nullptr, &this->renderFinishedSemaphoreCollection[i]) != VK_SUCCESS ||
			vkCreateFence(this->logicalDevice,     &fenceInfo,     nullptr, &this->inFlightFences[i]) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create semaphores for a frame!");
		}
	}
}
