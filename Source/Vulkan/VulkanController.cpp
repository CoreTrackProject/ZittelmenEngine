#include "VulkanController.h"

#include <QImage>

VulkanController::VulkanController()
{

	//// Default data (will be removed)
	//{

	//	this->tmpImage = QImage("D:/coretrack_devel/texture.jpg");
	//	this->imageTexture = VulkanTexture::NewTexture(
	//		this->vulkanDevice->GetPhysicalDevice(),
	//		this->vulkanDevice->GetLogicalDevice(),
	//		this->tmpImage
	//	);

	//	this->vertexCollection = VulkanVertexData::GetQuadVertexCollection();
	//	this->indexCollection = VulkanVertexData::GetQuadVertexIndexCollection();

	//}


}

VulkanController::~VulkanController()
{
	this->Destroy();
}

void VulkanController::SetTargetRenderSurface(WId target)
{
	this->target = target;
}

void VulkanController::ResizeTargetRenderSurface(uint32_t width, uint32_t height)
{

	//TODO: improve resizing logic, because atm all vulkan objects get destroyed and recreated

	if (this->vulkanDevice.get() != nullptr) {
		vkDeviceWaitIdle(this->vulkanDevice->GetLogicalDevice());
	}

	this->width  = width;
	this->height = height;

	this->Destroy();
	this->Initialize();
}

void VulkanController::Initialize()
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

	// Create image texture
	/*auto image = QImage("D:/coretrack_devel/texture.jpg");*/
	
	// Convert from QImage to Vulkan Texture
	this->imageTexture = VulkanTexture::NewTexture(
		this->vulkanDevice->GetPhysicalDevice(),
		this->vulkanDevice->GetLogicalDevice(),
		this->imageData
	);

	// Vulkan Uniform Buffer
	this->initVulkanUniform();



	// Setup depth buffering (To be relocated)
	{
		// Needs to be created before the graphics pipeline
		this->depthTexture = VulkanTexture::NewDepthTexture(
			this->vulkanDevice->GetPhysicalDevice(),
			this->vulkanDevice->GetLogicalDevice(),
			this->width, this->height);
	}
	

	// Vulkan Graphicspipeline
	this->initVulkanGraphicsPipeline();

	// Vulkan Command
	this->initVulkanCommand();



	// Setup depth buffering (To be relocated)
	{
		// Transform the image to the correct layout
		this->command->TransitionImageLayout(
			this->depthTexture->GetImage(),
			VulkanUtils::FindDepthFormat(this->vulkanDevice->GetPhysicalDevice()),
			VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
			VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}




	// Upload Vertex and Texture data
	{
		// TODO: create function for uploading vertex data (with buffers or directly)
		// Only after calling this function "this->command->getDrawCommandBufferCollection()" can be used
		//auto vertexData = VulkanVertexData::GetQuadVertexCollection();
		//auto indexData  = VulkanVertexData::GetQuadVertexIndexCollection();

		this->command->UploadVertexData(this->vertexCollection, this->indexCollection);
		this->command->UploadImage(this->imageTexture);
	}



	// Vulkan Runtime
	this->initVulkanRuntime();

	// Infos about lambdas:
	// https://de.cppreference.com/w/cpp/language/lambda
	// https://www.reddit.com/r/cpp/comments/6tgi25/binding_stdfunction_to_member_functions/

}

void VulkanController::Destroy()
{
	// Destroy all vulkan objects in reversed order

	this->destroyVulkanRuntime();
	this->destroyVulkanCommand();
	this->destroyVulkanGraphicsPipeline();

	this->depthTexture.reset();

	this->destroyVulkanUnform();

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
	this->instance = std::make_unique<VulkanInstance>(this->enableValidation);
}

void VulkanController::destroyVulkanInstance()
{
	this->instance.reset();
}

void VulkanController::initVulkanDebug()
{
	if (this->enableValidation) {
		this->vulkanDebug = std::make_unique<VulkanDebug>(this->instance->GetInstance());
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
	this->vulkanDevice = std::make_unique<VulkanDevice>(this->instance->GetInstance());
}

void VulkanController::destroyVulkanDevice()
{
	this->vulkanDevice.reset();
}

void VulkanController::initVulkanWindow()
{
	this->window = std::make_unique<VulkanWindow>(this->instance->GetInstance(), this->target);
}

void VulkanController::destroyVulkanWindow()
{
	this->window.reset();
}

void VulkanController::initVulkanSwapchain()
{
	this->swapchain = std::make_unique<VulkanSwapchain>(
		this->vulkanDevice->GetPhysicalDevice(),
		this->vulkanDevice->GetLogicalDevice(),
		this->vulkanDevice->GetPhysicalDeviceInfo(
			this->vulkanDevice->GetPhysicalDevice()
		),
		this->window->GetSurface(),
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
	this->shader = std::make_unique<VulkanShader>(this->vulkanDevice->GetLogicalDevice());
}

void VulkanController::destroyVulkanShader()
{
	this->shader.reset();
}

void VulkanController::initVulkanGraphicsPipeline()
{

	this->graphicsPipeline = std::make_unique<VulkanGraphicsPipeline>(
		this->vulkanDevice->GetPhysicalDevice(),
		this->vulkanDevice->GetLogicalDevice(),
		this->shader->GetVertexShaderModule(),
		this->shader->GetFragmentShaderModule(),
		this->swapchain->GetSwapchainExtent2D(),
		this->swapchain->GetSwapchainImageFormat(),
		this->swapchain->GetImageCollection(),
		this->uniform->GetDescriptorSetLayout(),
		this->depthTexture->GetImageView()
		);

}

void VulkanController::destroyVulkanGraphicsPipeline()
{
	this->graphicsPipeline.reset();
}

void VulkanController::initVulkanCommand()
{
	this->command = std::make_unique<VulkanCommand>(
		this->vulkanDevice->GetPhysicalDevice(),
		this->vulkanDevice->GetLogicalDevice(),
		this->vulkanDevice->GetPhysicalDeviceInfo(
			this->vulkanDevice->GetPhysicalDevice()
		),
		this->graphicsPipeline->GetFramebufferCollection(),
		this->graphicsPipeline->GetRenderPass(),
		this->swapchain->GetSwapchainExtent2D(),
		this->graphicsPipeline->GetGraphicsPipeline(),
		this->graphicsPipeline->GetGraphicsPipelineLayout(),
		this->vulkanDevice->GetGraphicsQueue(),
		this->uniform->GetDescriptorSetCollection()
		);
}

void VulkanController::destroyVulkanCommand()
{
	this->command.reset();
}

void VulkanController::initVulkanRuntime()
{
	this->runtime = std::make_unique<VulkanRuntime>(
		this->vulkanDevice->GetLogicalDevice(),
		this->swapchain->GetSwapchain(),
		this->command->GetDrawCommandBufferCollection(),
		this->vulkanDevice->GetGraphicsQueue(),
		this->swapchain->GetPresentQueue(),
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
		this->vulkanDevice->GetPhysicalDevice(),
		this->vulkanDevice->GetLogicalDevice(),
		static_cast<uint32_t>(this->swapchain->GetImageCollection().size()),
		this->swapchain->GetSwapchainExtent2D(),
		this->imageTexture->GetImageView(),
		this->imageTexture->GetImageSampler()
		);
}

void VulkanController::destroyVulkanUnform()
{
	this->uniform.reset();
}


void VulkanController::RenderFrame()
{
	this->runtime->RenderFrame();
}

void VulkanController::ImportData(std::vector<VulkanVertex>& vertexCollection, std::vector<std::uint32_t>& indexCollection, std::shared_ptr<QImage>& imageData)
{
	this->vertexCollection = vertexCollection;
	this->indexCollection  = indexCollection;
	this->imageData = imageData;
}
