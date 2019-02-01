#include "VulkanBase.h"

VulkanBase::VulkanBase()
{

}

VulkanBase::~VulkanBase()
{
	this->isRenderActive = false;
	this->destroy();
}

void VulkanBase::setTargetRenderSurface(QWidget* target)
{
	this->targetRenderWindow = target;
}

void VulkanBase::resizeTargetRenderSurface(uint32_t width, uint32_t height)
{
	this->isRenderActive = false;

	this->destroy();
	this->init();

	this->isRenderActive = true;
}

void VulkanBase::init()
{

	this->enableValidation = true;
	this->instance = new VulkanInstance(this->enableValidation);

	if (this->enableValidation) {
		this->vulkanDebug = new VulkanDebug(this->instance->getInstance());
	}

	this->vulkanDevice = new VulkanDevice(this->instance->getInstance());

	if (this->targetRenderWindow != nullptr) {
		this->window = new VulkanWindow(this->instance->getInstance(), this->targetRenderWindow);

		VkPhysicalDevice* tmpDev = this->vulkanDevice->getPhysicalDevice();
		VkDevice* tmpLogicalDev  = this->vulkanDevice->getLogicalDevice();
		DeviceInfo* tmpDevInfo   = this->vulkanDevice->getPhysicalDeviceInfo(tmpDev);
		VkSurfaceKHR* tmpSurface = this->window->getSurface();

		this->swapchain = new VulkanSwapchain(
			tmpDev,
			tmpLogicalDev, 
			tmpDevInfo, 
			tmpSurface, 
			this->targetRenderWindow->size().width(), 
			this->targetRenderWindow->size().height()
		);

		

		this->shader = new VulkanShader(tmpLogicalDev);

		this->graphicsPipeline = new VulkanGraphicsPipeline(
			tmpLogicalDev,
			this->shader->getVertexShaderModule(),
			this->shader->getFragmentShaderModule(),
			this->swapchain->getSwapchainExtent2D(),
			this->swapchain->getSwapchainImageFormat(),
			this->swapchain->getImageCollection()
		);

		this->command = new VulkanCommand(
			*tmpLogicalDev, 
			*tmpDevInfo,
			*this->graphicsPipeline->getFramebufferCollection(),
			*this->graphicsPipeline->getRenderPass(),
			*this->swapchain->getSwapchainExtent2D(),
			*this->graphicsPipeline->getGraphicsPipeline()
		);

		// TODO: has to be improved -> move to vulkan swapchain/device
		vkGetDeviceQueue(*this->vulkanDevice->getLogicalDevice(), this->swapchain->getQueueFamilyPresentIdx(), 0, &this->presentQueue);

		this->init_semaphore();
		
	}
}

void VulkanBase::destroy()
{
	vkDestroySemaphore(*this->vulkanDevice->getLogicalDevice(), renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(*this->vulkanDevice->getLogicalDevice(), imageAvailableSemaphore, nullptr);

	if (this->targetRenderWindow != nullptr) {
		delete this->command;
		delete this->graphicsPipeline;
		delete this->shader;
		delete this->swapchain;
		delete this->window;
	}

	delete this->vulkanDevice;
	if (this->enableValidation) {
		delete this->vulkanDebug;
	}

	delete this->instance;
}

void VulkanBase::init_semaphore()
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(*this->vulkanDevice->getLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(*this->vulkanDevice->getLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {

		throw std::runtime_error("failed to create semaphores!");
	}

}

void VulkanBase::renderFrame()
{
	if (!this->isRenderActive) {
		return;
	}

	//vkWaitForFences(*this->vulkanDevice->getLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	//vkResetFences(*this->vulkanDevice->getLogicalDevice(), 1, &inFlightFences[currentFrame]);

	uint32_t imageIndex = 0;
	vkAcquireNextImageKHR(
		*this->vulkanDevice->getLogicalDevice(), 
		this->swapchain->getSwapchain(), 
		std::numeric_limits<uint64_t>::max(), 
		imageAvailableSemaphore, 
		VK_NULL_HANDLE, 
		&imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &this->command->getCommandBufferCollection()[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;


	
	VkResult res = vkQueueSubmit(this->vulkanDevice->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
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
		throw std::runtime_error("failed to submit to present queu!");
	}



}
