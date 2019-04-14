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
	this->initVulkanInstance();

	// Vulkan Debug
	if (this->enableValidation) {
		this->initVulkanDebug();
	}

	// Vulkan Device
	this->initVulkanDevice();

	// Vulkan Window
	this->initVulkanWindow();

	// Vulkan Swapchain
	this->initVulkanSwapchain();

	// Vulkan Shader
	this->initVulkanShader();

	auto image = QImage("D:/coretrack_devel/texture.jpg");

	this->texture = VulkanTexture::newTexture(
		this->vulkanDevice->getPhysicalDevice(),
		this->vulkanDevice->getLogicalDevice(),
		image
	);

	// Vulkan Uniform Buffer
	this->initVulkanUniform();

	// Vulkan Graphicspipeline
	this->initVulkanGraphicsPipeline();

	// Vulkan Command
	this->initVulkanCommand();

	// TODO: create function for uploading vertex data (with buffers or directly)
	// Only after calling this function "this->command->getDrawCommandBufferCollection()" can be used
	auto vertexData = this->vertex.getQuadVertexCollection();
	auto indexData = this->vertex.getQuadVertexIndexCollection();

	this->command->uploadVertexData(vertexData, indexData);
	this->command->uploadImage(this->texture);

	// Vulkan Runtime
	this->initVulkanRuntime();

	// Infos about lambdas:
	// https://de.cppreference.com/w/cpp/language/lambda
	// https://www.reddit.com/r/cpp/comments/6tgi25/binding_stdfunction_to_member_functions/

}

void VulkanController::destroy()
{
	// Destroy all vulkan objects in reversed order

	this->destroyVulkanRuntime();
	this->destroyVulkanCommand();
	this->destroyVulkanGraphicsPipeline();
	this->destroyVulkanUnform();

	this->texture.reset();

	this->destroyVulkanShader();
	this->destroyVulkanSwapchain();
	this->destroyVulkanWindow();
	this->destroyVulkanDevice();
	if (this->enableValidation) {
		this->destroyVulkanDebug();
	}
	this->destroyVulkanInstance();
}

void VulkanController::initVulkanInstance()
{
	this->instance = std::make_unique<VulkanInstance>(this->enableValidation);
}

void VulkanController::destroyVulkanInstance()
{
	this->instance.reset();
}

void VulkanController::initVulkanDebug()
{
	if (this->enableValidation) {
		this->vulkanDebug = std::make_unique<VulkanDebug>(this->instance->getInstance());
	}
}

void VulkanController::destroyVulkanDebug()
{
	if (this->enableValidation) {
		this->vulkanDebug.reset();
	}
}

void VulkanController::initVulkanDevice()
{
	this->vulkanDevice = std::make_unique<VulkanDevice>(this->instance->getInstance());
}

void VulkanController::destroyVulkanDevice()
{
	this->vulkanDevice.reset();
}

void VulkanController::initVulkanWindow()
{
	this->window = std::make_unique<VulkanWindow>(this->instance->getInstance(), this->target);
}

void VulkanController::destroyVulkanWindow()
{
	this->window.reset();
}

void VulkanController::initVulkanSwapchain()
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

void VulkanController::destroyVulkanSwapchain()
{
	this->swapchain.reset();
}

void VulkanController::initVulkanShader()
{
	this->shader = std::make_unique<VulkanShader>(this->vulkanDevice->getLogicalDevice());
}

void VulkanController::destroyVulkanShader()
{
	this->shader.reset();
}

void VulkanController::initVulkanGraphicsPipeline()
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

void VulkanController::destroyVulkanGraphicsPipeline()
{
	this->graphicsPipeline.reset();
}

void VulkanController::initVulkanCommand()
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
}

void VulkanController::destroyVulkanCommand()
{
	this->command.reset();
}

void VulkanController::initVulkanRuntime()
{
	this->runtime = std::make_unique<VulkanRuntime>(
		this->vulkanDevice->getLogicalDevice(),
		this->swapchain->getSwapchain(),
		this->command->getDrawCommandBufferCollection(),
		this->vulkanDevice->getGraphicsQueue(),
		this->swapchain->getPresentQueue(),
		this->uniform // Temp. so that during runtime the updateUniformData() can be called (should be by a callback solution)
		);
}

void VulkanController::destroyVulkanRuntime()
{
	this->runtime.reset();
}

void VulkanController::initVulkanUniform()
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

void VulkanController::destroyVulkanUnform()
{
	this->uniform.reset();
}


void VulkanController::renderFrame()
{
	this->runtime->renderFrame();
}
