#include "VulkanController.h"

#include <QImage>

VulkanController::VulkanController()
{

}

VulkanController::~VulkanController()
{
	this->destroy();
}

void VulkanController::setTargetRenderSurface(WId target)
{
	this->target = target;
}

void VulkanController::resizeTargetRenderSurface(uint32_t width, uint32_t height)
{
	this->width  = width;
	this->height = height;

	this->destroy();
	this->initialize();
}

void VulkanController::initialize()
{
	this->enableValidation = true;

	if (this->target == 0) {
		throw std::exception("No target render Window specified");
		return;
	}

	if (this->width <= 0 || this->height <= 0) {
		return;
	}

	// Vulkan Instance
	{
		this->instance = std::make_unique<VulkanInstance>(this->enableValidation);
	}

	// Vulkan Debug
	{
		if (this->enableValidation) {
			this->vulkanDebug = std::make_unique<VulkanDebug>(this->instance->getInstance());
		}
	}

	// Vulkan Device
	{
		this->vulkanDevice = std::make_unique<VulkanDevice>(this->instance->getInstance());
	}
	
	// Vulkan Window
	{
		this->window = std::make_unique<VulkanWindow>(this->instance->getInstance(), this->target);
	}

	// Vulkan Swapchain
	{
		this->swapchain = std::make_unique<VulkanSwapchain>(
			this->vulkanDevice->getPhysicalDevice(),
			this->vulkanDevice->getLogicalDevice(),
			this->vulkanDevice->getPhysicalDeviceInfo(
				this->vulkanDevice->getPhysicalDevice()
			),
			this->window->getSurface(),
			this->width,
			this->height
		);
	}

	// Vulkan Shader
	{
		this->shader = std::make_unique<VulkanShader>(this->vulkanDevice->getLogicalDevice());
	}

	auto image = QImage("D:/coretrack_devel/texture.jpg");

	this->texture = VulkanTexture::newTexture(
		this->vulkanDevice->getPhysicalDevice(),
		this->vulkanDevice->getLogicalDevice(),
		image
	);


	// Vulkan Uniform Buffer
	{
		this->uniform = std::make_shared<VulkanUniform>(
			this->vulkanDevice->getPhysicalDevice(),
			this->vulkanDevice->getLogicalDevice(),
			static_cast<uint32_t>(this->swapchain->getImageCollection().size()),
			this->swapchain->getSwapchainExtent2D(),
			this->texture->getImageView(),
			this->texture->getImageSampler()
			);
	}
	
	// Vulkan Graphicspipeline
	{
		this->graphicsPipeline = std::make_unique<VulkanGraphicsPipeline>(
			this->vulkanDevice->getLogicalDevice(),
			this->shader->getVertexShaderModule(),
			this->shader->getFragmentShaderModule(),
			this->swapchain->getSwapchainExtent2D(),
			this->swapchain->getSwapchainImageFormat(),
			this->swapchain->getImageCollection(),
			this->uniform->getDescriptorSetLayout()
		);
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
		this->command = std::make_unique<VulkanCommand>(
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
			this->vulkanDevice->getGraphicsQueue(),
			this->uniform->getDescriptorSetCollection()
		);


        // TODO: create function for uploading vertex data (with buffers or directly)
		// Only after calling this function "this->command->getDrawCommandBufferCollection()" can be used
        auto vertexData = this->vertex.getQuadVertexCollection();
        auto indexData  = this->vertex.getQuadVertexIndexCollection();

		
        this->command->uploadVertexData(vertexData, indexData);
		this->command->uploadImage(this->texture);
        
	}

	// Vulkan Runtime
	{
		this->runtime = std::make_unique<VulkanRuntime>(
			this->vulkanDevice->getLogicalDevice(),
			this->swapchain->getSwapchain(),
			this->command->getDrawCommandBufferCollection(),
			this->vulkanDevice->getGraphicsQueue(),
			this->swapchain->getPresentQueue(),
			this->uniform // Temp. so that during runtime the updateUniformData() can be called (should be by a callback solution)
		);

		// Infos about lambdas:
		// https://de.cppreference.com/w/cpp/language/lambda
		// https://www.reddit.com/r/cpp/comments/6tgi25/binding_stdfunction_to_member_functions/

	}

}

void VulkanController::destroy()
{

	this->runtime.reset();
	this->command.reset();
	//this->factory.reset();
	this->graphicsPipeline.reset();
	this->uniform.reset();
	this->texture.reset();
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
