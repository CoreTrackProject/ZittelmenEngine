#include "VulkanBase.h"

VulkanBase::VulkanBase()
{

}

VulkanBase::~VulkanBase()
{
	this->isRenderActive = false;
	this->destroy();
}

void VulkanBase::setTargetRenderSurface(QWidget *targetWindow)
{
	this->targetRenderWindow = targetWindow;
}

void VulkanBase::resizeTargetRenderSurface(uint32_t width, uint32_t height)
{
	this->isRenderActive = false;

	this->destroy();
	this->initialize();

	this->isRenderActive = true;
}

void VulkanBase::initialize()
{

	this->enableValidation = true;
	
	this->instance.reset(new VulkanInstance(this->enableValidation));

	if (this->enableValidation) {
		this->vulkanDebug.reset(new VulkanDebug(this->instance->getInstance()));
	}

	this->vulkanDevice.reset(new VulkanDevice(this->instance->getInstance()));

	if (this->targetRenderWindow != nullptr) {
		this->window.reset(new VulkanWindow(this->instance->getInstance(), this->targetRenderWindow));

		VkPhysicalDevice &tmpDev = this->vulkanDevice->getPhysicalDevice();
		VkDevice &tmpLogicalDev  = this->vulkanDevice->getLogicalDevice();
		DeviceInfo &tmpDevInfo   = this->vulkanDevice->getPhysicalDeviceInfo(tmpDev);
		VkSurfaceKHR &tmpSurface = this->window->getSurface();

		this->swapchain.reset(new VulkanSwapchain(
			tmpDev,
			tmpLogicalDev, 
			tmpDevInfo, 
			tmpSurface, 
			this->targetRenderWindow->size().width(), 
			this->targetRenderWindow->size().height()
		));

		

		this->shader.reset(new VulkanShader(tmpLogicalDev));

		this->graphicsPipeline.reset(new VulkanGraphicsPipeline(
			tmpLogicalDev,
			this->shader->getVertexShaderModule(),
			this->shader->getFragmentShaderModule(),
			this->swapchain->getSwapchainExtent2D(),
			this->swapchain->getSwapchainImageFormat(),
			this->swapchain->getImageCollection()
		));

		this->command.reset(new VulkanCommand(
			tmpLogicalDev, 
			tmpDevInfo,
			this->graphicsPipeline->getFramebufferCollection(),
			this->graphicsPipeline->getRenderPass(),
			this->swapchain->getSwapchainExtent2D(),
			this->graphicsPipeline->getGraphicsPipeline()
		));

		// TODO: has to be improved -> move to vulkan [swapchain]/device
		vkGetDeviceQueue(this->vulkanDevice->getLogicalDevice(), this->swapchain->getQueueFamilyPresentIdx(), 0, &this->presentQueue);

		this->init_syncobjects();
		
	}
}

void VulkanBase::destroy()
{
	this->renderRunning = false;


	if (this->vulkanDevice.get() != nullptr) {
		vkDeviceWaitIdle(this->vulkanDevice->getLogicalDevice());
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(this->vulkanDevice->getLogicalDevice(), renderFinishedSemaphoreCollection[i], nullptr);
			vkDestroySemaphore(this->vulkanDevice->getLogicalDevice(), imageAvailableSemaphoreCollection[i], nullptr);
			vkDestroyFence(this->vulkanDevice->getLogicalDevice(), inFlightFences[i], nullptr);
		}
	}


	if (this->targetRenderWindow != nullptr) {
		this->command.reset();
		this->graphicsPipeline.reset();
		this->shader.reset();
		this->swapchain.reset();
		this->window.reset();
	}

	this->vulkanDevice.reset();
	if (this->enableValidation) {
		 this->vulkanDebug.reset();
	}

	this->instance.reset();

}

void VulkanBase::init_syncobjects()
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
		if (vkCreateSemaphore(this->vulkanDevice->getLogicalDevice(), &semaphoreInfo, nullptr, &this->imageAvailableSemaphoreCollection[i]) != VK_SUCCESS ||
			vkCreateSemaphore(this->vulkanDevice->getLogicalDevice(), &semaphoreInfo, nullptr, &this->renderFinishedSemaphoreCollection[i]) != VK_SUCCESS ||
			vkCreateFence(this->vulkanDevice->getLogicalDevice(),     &fenceInfo, nullptr,     &this->inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphores for a frame!");
		}
	}
}

void VulkanBase::renderFrame()
{
	if (!this->isRenderActive) {
		return;
	}

	VkResult res = vkWaitForFences(this->vulkanDevice->getLogicalDevice(), 1, &this->inFlightFences[this->currentFrame], VK_FALSE, 1000000000);
	if (res != VkResult::VK_SUCCESS) {
		return;
	}

	vkResetFences(this->vulkanDevice->getLogicalDevice(), 1,   &this->inFlightFences[this->currentFrame]);

	uint32_t imageIndex = 0;
	vkAcquireNextImageKHR(
		this->vulkanDevice->getLogicalDevice(), 
		this->swapchain->getSwapchain(),
		std::numeric_limits<uint64_t>::max(), 
		this->imageAvailableSemaphoreCollection[this->currentFrame],
		VK_NULL_HANDLE, 
		&imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { this->imageAvailableSemaphoreCollection[this->currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &this->command->getCommandBufferCollection()[imageIndex];

	VkSemaphore signalSemaphores[] = { this->renderFinishedSemaphoreCollection[this->currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	res = vkQueueSubmit(this->vulkanDevice->getGraphicsQueue(), 1, &submitInfo, this->inFlightFences[currentFrame]);
	if (res != VK_SUCCESS) {
		qDebug("failed to submit draw command buffer!");
		
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { this->swapchain->getSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	res = vkQueuePresentKHR(this->presentQueue, &presentInfo);
	if (res != VK_SUCCESS) {
		qDebug("failed to submit to present queu!");
		
	}

	this->currentFrame = (this->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

