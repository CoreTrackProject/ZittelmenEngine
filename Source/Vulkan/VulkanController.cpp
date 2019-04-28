#include "VulkanController.h"

VulkanController::VulkanController()
{
	this->currStatus = VulkanControllerStatus::VC_Created;
}

VulkanController::~VulkanController()
{
	this->Destroy();
}



void VulkanController::SetTargetRenderSurface(WId target, std::uint32_t width, std::uint32_t height)
{
	this->target = target;
	this->width  = width;
	this->height = height;
}

void VulkanController::ResizeTargetRenderSurface(uint32_t newWidth, uint32_t newHeight)
{

	if (this->currStatus != VulkanControllerStatus::VC_Ready) {
		return;
	}
	this->currStatus = VulkanControllerStatus::VC_Resizing;

	//TODO: improve resizing logic, because atm all vulkan objects get destroyed and recreated

	if (this->vulkanDevice.get() != nullptr) {
		vkDeviceWaitIdle(this->vulkanDevice->GetLogicalDevice());
	}


	this->width = newWidth;
	this->height = newHeight;


	// Destroy objects
	{
		// Destroy all vulkan objects in reversed order
		this->destroyVulkanRuntime();
		this->destroyVulkanSwapchain();
	}

	// Init objects
	{
		this->initVulkanSwapchain();

		// Use swapchain extent instead of newWidth/newHeight
		this->uniform->UpdateViewportDimension(this->swapchain->GetSwapchainExtent2D().width, this->swapchain->GetSwapchainExtent2D().height);

		// Update vulkancommand because swapchain vulkan objects had changed
		this->command->UpdateFramebufferCollection(this->swapchain->GetFramebufferCollection(), this->swapchain->GetSwapchainExtent2D());
		this->command->UpdateRenderpass(this->swapchain->GetRenderpass());

		this->command->TransitionImageLayout(
			this->swapchain->GetDepthTexture()->GetImage(),
			VulkanUtils::FindDepthFormat(this->vulkanDevice->GetPhysicalDevice()),
			VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		);
		
		this->initVulkanRuntime();

	}

	this->currStatus = VulkanControllerStatus::VC_Ready;
}

void VulkanController::RenderFrame()
{
	// VulkanController needs to be ready for rendering
	if (this->currStatus != VulkanControllerStatus::VC_Ready) {
		return;
	} else if (this->width <= 0 || this->height <= 0) {
		// Device will be lost if the viewport has zero width or legth
		// something by the renderpass failed
		return;
	}


	this->currStatus = VulkanControllerStatus::VC_Rendering;
	this->runtime->RenderFrame();
	this->currStatus = VulkanControllerStatus::VC_Ready;

}

void VulkanController::ImportData(std::vector<VulkanVertex>& vertexCollection, std::vector<std::uint32_t>& indexCollection, std::shared_ptr<QImage>& imageData)
{
	this->vertexCollection = vertexCollection;
	this->indexCollection = indexCollection;
	this->imageData = imageData;
}

void VulkanController::Initialize() 
{
	this->currStatus = VulkanControllerStatus::VC_Initializing;
	this->enableValidation = true;

	if (this->target == 0) {
		throw std::exception("No target render Window specified");
		return;
	}
	this->initVulkanInstance();
	if (this->enableValidation) {
		this->initVulkanDebug();
	}
	this->initVulkanDevice();

	this->initVulkanWindow();

	// Setup depth buffering (To be relocated)
	{
		// Convert from QImage to Vulkan Texture
		this->imageTexture = VulkanTexture::NewTexture(
			this->vulkanDevice->GetPhysicalDevice(),
			this->vulkanDevice->GetLogicalDevice(),
			this->imageData
		);
	}
	this->initVulkanSwapchain();
	this->initVulkanShader();

	// Create image texture
	
	this->initVulkanUniform();
	this->initVulkanGraphicsPipeline();

	this->initVulkanCommand();

	// Setup depth buffering (To be relocated)
	{
		// Transform the image to the correct layout
		this->command->TransitionImageLayout(
			this->swapchain->GetDepthTexture()->GetImage(),
			VulkanUtils::FindDepthFormat(this->vulkanDevice->GetPhysicalDevice()),
			VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	// Upload Vertex and Texture data
	{
		// TODO: create function for uploading vertex data (with buffers or directly)
		// Only after calling this function "this->command->getDrawCommandBufferCollection()" can be used
		this->uploadContent();
	}

	this->initVulkanRuntime();

	// Infos about lambdas:
	// https://de.cppreference.com/w/cpp/language/lambda
	// https://www.reddit.com/r/cpp/comments/6tgi25/binding_stdfunction_to_member_functions/

	this->currStatus = VulkanControllerStatus::VC_Ready;
}

void VulkanController::Destroy() 
{
	if (this->currStatus != VulkanControllerStatus::VC_Ready) {
		return;
	}

	// Destroy all vulkan objects in reversed order

	this->destroyVulkanRuntime();
	this->destroyVulkanCommand();
	this->destroyVulkanGraphicsPipeline();
	this->destroyVulkanUniform();

	this->imageTexture.reset();

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
	VulkanInstanceCreateInfo createInfo = {};
	createInfo.enableValidation = this->enableValidation;

	this->instance = std::make_unique<VulkanInstance>(createInfo);
}

void VulkanController::destroyVulkanInstance()
{
	this->instance.reset();
}

void VulkanController::initVulkanDebug()
{
	VulkanDebugCreateInfo createInfo = {};
	createInfo.instance = this->instance->GetInstance();

	if (this->enableValidation) {
		this->vulkanDebug = std::make_unique<VulkanDebug>(createInfo);
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
	VulkanDeviceCreateInfo createInfo = {};
	createInfo.instance = this->instance->GetInstance();

	this->vulkanDevice = std::make_unique<VulkanDevice>(createInfo);
}

void VulkanController::destroyVulkanDevice()
{
	this->vulkanDevice.reset();
}

void VulkanController::initVulkanWindow()
{
	VulkanWindowCreateInfo createInfo = {};
	createInfo.instance            = this->instance->GetInstance();
	createInfo.targetRenderSurface = this->target;

	this->window = std::make_unique<VulkanWindow>(createInfo);
}

void VulkanController::destroyVulkanWindow()
{
	this->window.reset();
}

void VulkanController::initVulkanSwapchain()
{
	VulkanSwapchainCreateInfo createInfo = {};
	createInfo.device        = this->vulkanDevice->GetPhysicalDevice();
	createInfo.logicalDevice = this->vulkanDevice->GetLogicalDevice();
	createInfo.deviceInfo    = 
		this->vulkanDevice->GetPhysicalDeviceInfo(
			this->vulkanDevice->GetPhysicalDevice()
		);
	createInfo.surface       = this->window->GetSurface();
	createInfo.width         = this->width;
	createInfo.height        = this->height;

	this->swapchain = std::make_unique<VulkanSwapchain>(createInfo);
}

void VulkanController::destroyVulkanSwapchain()
{
	this->swapchain.reset();
}

void VulkanController::initVulkanShader()
{
	VulkanShaderCreateInfo createInfo = {};
	createInfo.logicalDevice = this->vulkanDevice->GetLogicalDevice();

	this->shader = std::make_unique<VulkanShader>(createInfo);
}

void VulkanController::destroyVulkanShader()
{
	this->shader.reset();
}

void VulkanController::initVulkanGraphicsPipeline()
{
	VulkanGraphicsPipelineCreateInfo createInfo = {};
	createInfo.physicalDevice       = this->vulkanDevice->GetPhysicalDevice();
	createInfo.logicalDevice        = this->vulkanDevice->GetLogicalDevice();
	createInfo.vertexShaderModule   = this->shader->GetVertexShaderModule();
	createInfo.fragmentShaderModule = this->shader->GetFragmentShaderModule();
	createInfo.swapchainExtent      = this->swapchain->GetSwapchainExtent2D();
	createInfo.swapchainImageFormat = this->swapchain->GetSwapchainImageFormat();
	createInfo.descriptorSetLayout  = this->uniform->GetDescriptorSetLayout();
	createInfo.renderpass           = this->swapchain->GetRenderpass();

	this->graphicsPipeline = std::make_unique<VulkanGraphicsPipeline>(createInfo);
}

void VulkanController::destroyVulkanGraphicsPipeline()
{
	this->graphicsPipeline.reset();
}

void VulkanController::initVulkanCommand()
{

	VulkanCommandCreateInfo createInfo = {};
	createInfo.physicalDev = this->vulkanDevice->GetPhysicalDevice();
	createInfo.logicalDevice = this->vulkanDevice->GetLogicalDevice();
	createInfo.deviceInfo = 
		this->vulkanDevice->GetPhysicalDeviceInfo(
			this->vulkanDevice->GetPhysicalDevice()
	);
	createInfo.frameBufferCollection = this->swapchain->GetFramebufferCollection();
	createInfo.renderpass = this->swapchain->GetRenderpass();
	createInfo.swapchainExtent = this->swapchain->GetSwapchainExtent2D();
	createInfo.graphicsPipeline = this->graphicsPipeline->GetGraphicsPipeline();
	createInfo.pipelineLayout =	this->graphicsPipeline->GetGraphicsPipelineLayout();
	createInfo.graphicsQueue = 	this->vulkanDevice->GetGraphicsQueue();
	createInfo.descriptorSetCollection = this->uniform->GetDescriptorSetCollection();


	this->command = std::make_unique<VulkanCommand>(createInfo);

}

void VulkanController::destroyVulkanCommand()
{
	this->command.reset();
}

void VulkanController::initVulkanRuntime()
{
	VulkanRuntimeCreateInfo createInfo = {};
	createInfo.logicalDevice           = this->vulkanDevice->GetLogicalDevice();
	createInfo.swapchain               = this->swapchain->GetSwapchain();
	createInfo.commandBufferCollection = this->command->GetDrawCommandBufferCollection();
	createInfo.graphicsQueue           = this->vulkanDevice->GetGraphicsQueue();
	createInfo.presentQueue            = this->swapchain->GetPresentQueue();
	createInfo.uniform                 = this->uniform; // Temp. so that during runtime the updateUniformData() can be called (should be by a callback solution)

	this->runtime = std::make_unique<VulkanRuntime>(createInfo);
}

void VulkanController::destroyVulkanRuntime()
{
	this->runtime.reset();
}

void VulkanController::initVulkanUniform()
{
	VulkanUniformCreateInfo createInfo = {};
	createInfo.physicalDevice = this->vulkanDevice->GetPhysicalDevice();
	createInfo.logicalDevice = this->vulkanDevice->GetLogicalDevice();
	createInfo.swapChainImageCollectionSize = static_cast<uint32_t>(this->swapchain->GetImageCollection().size());
	createInfo.imageView = this->imageTexture->GetImageView();
	createInfo.imageSampler = this->imageTexture->GetImageSampler();
	createInfo.width = this->swapchain->GetSwapchainExtent2D().width;
	createInfo.height = this->swapchain->GetSwapchainExtent2D().height;

	// TODO remove GetSwapchainExtent2D() because it is only used for projection calculation
	this->uniform = std::make_shared<VulkanUniform>(createInfo);

}

void VulkanController::destroyVulkanUniform()
{
	this->uniform.reset();
}

void VulkanController::uploadContent()
{
	this->command->UploadVertexData(this->vertexCollection, this->indexCollection);
	this->command->UploadImage(this->imageTexture);
}
