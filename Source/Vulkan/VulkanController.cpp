#include "VulkanController.h"

VulkanController::VulkanController()
{

}

VulkanController::~VulkanController()
{
	this->destroy();
}

void VulkanController::setTargetRenderSurface(QWidget *targetWindow)
{
	this->targetRenderWindow = targetWindow;
}

void VulkanController::resizeTargetRenderSurface(uint32_t width, uint32_t height)
{
	this->destroy();
	this->initialize();
}

void VulkanController::initialize()
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

	// Vulkan Uniform Buffer
	{
		this->uniform.reset(
			new VulkanUniform(
				this->vulkanDevice->getPhysicalDevice(), 
				this->vulkanDevice->getLogicalDevice(),
				static_cast<uint32_t>(this->swapchain->getImageCollection().size()),
				this->swapchain->getSwapchainExtent2D()
			)
		);
	}
	
	// Vulkan Graphicspipeline
	{
		this->graphicsPipeline.reset(new VulkanGraphicsPipeline(
			this->vulkanDevice->getLogicalDevice(),
			this->shader->getVertexShaderModule(),
			this->shader->getFragmentShaderModule(),
			this->swapchain->getSwapchainExtent2D(),
			this->swapchain->getSwapchainImageFormat(),
			this->swapchain->getImageCollection(),
			this->uniform->getDescriptorSetLayout()
		));
	}

	// Vulkan Factory
	{
		/*this->factory.reset(
			new VulkanFactory(
				this->vulkanDevice->getPhysicalDevice(),
				this->vulkanDevice->getLogicalDevice()
			));*/

	}


	// Staging buffer notes:
	// Vertexbuffer is only in gpu
	// Stagingbuffer is created which is accessible from gpu and cpu
	// Vertex data is first stored in staging buffer
	// Staging buffer gets uploaded to vertex buffer in gpu with a vulkan command (vkCmd...BlaBlaBla....)
	

	// Vulkan Command
	{
		this->command.reset(new VulkanCommand(
			this->vulkanDevice->getPhysicalDevice(),
			this->vulkanDevice->getLogicalDevice(),
			this->vulkanDevice->getPhysicalDeviceInfo(
				this->vulkanDevice->getPhysicalDevice()
			),
			this->graphicsPipeline->getFramebufferCollection(),
			this->graphicsPipeline->getRenderPass(),
			this->swapchain->getSwapchainExtent2D(),
			this->graphicsPipeline->getGraphicsPipeline(),
			this->graphicsPipeline->getGraphicsPipelineLayout(),
			this->vulkanDevice->getTransferQueue(),
			this->uniform->getDescriptorSetCollection()
		));

		// TODO: create function for uploading vertex data (with buffers or directly)
		// Only after calling this function "this->command->getDrawCommandBufferCollection()" can be used
		this->command->uploadVertexData(this->vertex.getQuadVertexCollection(), this->vertex.getQuadVertexIndexCollection());

	}

	// Vulkan Runtime
	{
		this->runtime.reset(new VulkanRuntime(
			this->vulkanDevice->getLogicalDevice(),
			this->swapchain->getSwapchain(),
			this->command->getDrawCommandBufferCollection(), // TODO: rename this to getDrawCommandCollection(Vertex buffer params)
			this->vulkanDevice->getGraphicsQueue(),
			this->swapchain->getPresentQueue()
		));

		// Infos about lambdas:
		// https://de.cppreference.com/w/cpp/language/lambda
		// https://www.reddit.com/r/cpp/comments/6tgi25/binding_stdfunction_to_member_functions/

		//this->runtime->registerUpdateUBOCallback([&](uint32_t currFrameIdx) { this->uniform->updateUniformData(currFrameIdx); });

	}

}



void VulkanController::destroy()
{

	this->runtime.reset();
	this->command.reset();
	//this->factory.reset();
	this->graphicsPipeline.reset();
	this->uniform.reset();
	this->shader.reset();
	this->swapchain.reset();
	this->window.reset();

	this->vulkanDevice.reset();
	if (this->enableValidation) {
		this->vulkanDebug.reset();
	}

	this->instance.reset();
}

void VulkanController::renderFrame()
{
	this->runtime->renderFrame();
}