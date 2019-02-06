#include "VulkanBase.h"

VulkanBase::VulkanBase()
{

}

VulkanBase::~VulkanBase()
{

	this->destroy();
}

void VulkanBase::setTargetRenderSurface(QWidget *targetWindow)
{
	this->targetRenderWindow = targetWindow;
}

void VulkanBase::resizeTargetRenderSurface(uint32_t width, uint32_t height)
{
	this->destroy();
	this->initialize();

}

void VulkanBase::initialize()
{
	this->enableValidation = true;

	if (this->targetRenderWindow == nullptr) {
		throw std::exception("No target render Window specified");
		return;
	}

	// Vulkan Instance
	{
		this->instance.reset(new VulkanInstance(this->enableValidation));
	}

	// Vulkan Debug
	{
		if (this->enableValidation) {
			this->vulkanDebug.reset(new VulkanDebug(this->instance->getInstance()));
		}
	}

	// Vulkan Device
	{
		this->vulkanDevice.reset(new VulkanDevice(this->instance->getInstance()));
	}
	
	// Vulkan Window
	{
		this->window.reset(new VulkanWindow(this->instance->getInstance(), this->targetRenderWindow));
	}
	
	// Vulkan Swapchain
	{
		this->swapchain.reset(new VulkanSwapchain(
			this->vulkanDevice->getPhysicalDevice(),
			this->vulkanDevice->getLogicalDevice(),

			this->vulkanDevice->getPhysicalDeviceInfo(
				this->vulkanDevice->getPhysicalDevice()
			),

			this->window->getSurface(),
			this->targetRenderWindow->size().width(),
			this->targetRenderWindow->size().height()
		));
	}

	// Vulkan Shader
	{
		this->shader.reset(new VulkanShader(
			this->vulkanDevice->getLogicalDevice()
		));
	}
	
	// Vulkan Graphicspipeline
	{
		this->graphicsPipeline.reset(new VulkanGraphicsPipeline(
			this->vulkanDevice->getLogicalDevice(),
			this->shader->getVertexShaderModule(),
			this->shader->getFragmentShaderModule(),
			this->swapchain->getSwapchainExtent2D(),
			this->swapchain->getSwapchainImageFormat(),
			this->swapchain->getImageCollection()
		));
	}
	
	// Vulkan Command
	{
		this->command.reset(new VulkanCommand(
			this->vulkanDevice->getLogicalDevice(),
			this->vulkanDevice->getPhysicalDeviceInfo(
				this->vulkanDevice->getPhysicalDevice()
			),
			this->graphicsPipeline->getFramebufferCollection(),
			this->graphicsPipeline->getRenderPass(),
			this->swapchain->getSwapchainExtent2D(),
			this->graphicsPipeline->getGraphicsPipeline()
		));
	}
	
	// Vulkan Runtime
	{
		this->runtime.reset(new VulkanRuntime(
			this->vulkanDevice->getLogicalDevice(),
			this->swapchain->getSwapchain(),
			this->command->getCommandBufferCollection(),
			this->vulkanDevice->getGraphicsQueue(),
			this->swapchain->getPresentQueue()
		));
	}

}

void VulkanBase::destroy()
{

	this->runtime.reset();
	this->command.reset();
	this->graphicsPipeline.reset();
	this->shader.reset();
	this->swapchain.reset();
	this->window.reset();

	this->vulkanDevice.reset();
	if (this->enableValidation) {
		this->vulkanDebug.reset();
	}

	this->instance.reset();
}

void VulkanBase::renderFrame()
{
	this->runtime->renderFrame();
}

