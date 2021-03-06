#include "VulkanSwapchain.h"

VulkanSwapchain::VulkanSwapchain(VulkanSwapchainCreateInfo createInfo) : createInfo(createInfo)
{
	this->init_Swapchain();
	this->init_Imageviews();
	this->init_DepthTexture();

	this->init_Renderpass();
	this->init_Framebuffer();
}

VulkanSwapchain::~VulkanSwapchain()
{
	for (auto framebuffer : this->swapchainFramebufferCollection) {
		vkDestroyFramebuffer(this->createInfo.logicalDevice, framebuffer, nullptr);
	}

	vkDestroyRenderPass(this->createInfo.logicalDevice, this->renderPass, nullptr);

	for (Image image : this->imageCollection) {
		vkDestroyImageView(this->createInfo.logicalDevice, image.imageView, nullptr);
	}

	vkDestroySwapchainKHR(this->createInfo.logicalDevice, this->swapChain, nullptr);
}




VkExtent2D VulkanSwapchain::GetSwapchainExtent2D()
{
	return this->selectedExtent;
}

VkSurfaceFormatKHR VulkanSwapchain::GetSwapchainImageFormat()
{
	return this->selectedSurfaceFormat;
}

std::shared_ptr<VulkanTexture> VulkanSwapchain::GetDepthTexture()
{
	return this->depthTexture;
}

std::vector<Image> VulkanSwapchain::GetImageCollection()
{
	return this->imageCollection;
}

VkSwapchainKHR VulkanSwapchain::GetSwapchain()
{
	return this->swapChain;
}

VkQueue VulkanSwapchain::GetPresentQueue()
{
	return this->presentQueue;
}

VkRenderPass VulkanSwapchain::GetRenderpass()
{
	return this->renderPass;
}

std::vector<VkFramebuffer> VulkanSwapchain::GetFramebufferCollection()
{
	return this->swapchainFramebufferCollection;
}

std::uint32_t VulkanSwapchain::GetQueueFamilyPresentIdx()
{
	return this->queueFamilyPresentIdx;
}



void VulkanSwapchain::init_Swapchain()
{
	this->querySwapChainRelatedInfo();

	this->selectedSurfaceFormat  = this->chooseSwapSurfaceFormat(this->details.formats);
	this->selectedPresentMode    = this->chooseSwapPresentMode(this->details.presentModes);
	this->selectedExtent    	 = this->chooseSwapExtent(this->details.capabilities);

	uint32_t imageCount = this->details.capabilities.minImageCount + 1;
	if (this->details.capabilities.maxImageCount > 0 && imageCount > this->details.capabilities.maxImageCount) {
		imageCount = this->details.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = this->createInfo.surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = this->selectedSurfaceFormat.format;
	createInfo.imageColorSpace = this->selectedSurfaceFormat.colorSpace;
	createInfo.imageExtent = this->selectedExtent;

	/* 
		Link: https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
	*/
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Get queue family which supports presenting
	{

		{
			VkBool32 supprtPresent = VK_FALSE;
			for (uint32_t i = 0; i < this->createInfo.deviceInfo.queueFamilyCount; i++) {
				VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(this->createInfo.device, i, this->createInfo.surface, &supprtPresent);
				if (res != VkResult::VK_SUCCESS) {
					throw std::runtime_error("Failed to get physical device surface support information.");
				}
				if (supprtPresent == VK_TRUE) {
					this->queueFamilyPresentIdx = i;
					break;
				}
			}
			vkGetDeviceQueue(this->createInfo.logicalDevice, this->GetQueueFamilyPresentIdx(), 0, &this->presentQueue);
		}

		uint32_t queueFamilyIndices[] = { this->createInfo.deviceInfo.queueFamilyIndexes.graphics, this->queueFamilyPresentIdx };

		if (this->createInfo.deviceInfo.queueFamilyIndexes.graphics != this->queueFamilyPresentIdx) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {

			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}
	}

	createInfo.preTransform   = this->details.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = this->selectedPresentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult res = vkCreateSwapchainKHR(this->createInfo.logicalDevice, &createInfo, nullptr, &this->swapChain);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

}

void VulkanSwapchain::querySwapChainRelatedInfo()
{

	VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->createInfo.device, this->createInfo.surface, &details.capabilities);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to get physical device surface capabilities.");
	}

	// Get surface formats
	{
		uint32_t formatCount;
		VkResult res = vkGetPhysicalDeviceSurfaceFormatsKHR(this->createInfo.device, this->createInfo.surface, &formatCount, nullptr);
		if (res != VkResult::VK_SUCCESS) {
			throw std::runtime_error("Failed to get physical device surface formats information.");
		}

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			res = vkGetPhysicalDeviceSurfaceFormatsKHR(this->createInfo.device, this->createInfo.surface, &formatCount, details.formats.data());
			if (res != VkResult::VK_SUCCESS) {
				throw std::runtime_error("Failed to get physical device surface formats information.");
			}
		}
	}
	
	// Get Presentmodes
	{
		uint32_t presentModeCount;
		VkResult res = vkGetPhysicalDeviceSurfacePresentModesKHR(this->createInfo.device, this->createInfo.surface, &presentModeCount, nullptr);
		if (res != VkResult::VK_SUCCESS) {
			throw std::runtime_error("Failed to get physical device surface present modes information.");
		}
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			res = vkGetPhysicalDeviceSurfacePresentModesKHR(this->createInfo.device, this->createInfo.surface, &presentModeCount, details.presentModes.data());
			if (res != VkResult::VK_SUCCESS) {
				throw std::runtime_error("Failed to get physical device surface present modes information.");
			}
		}
		
	} 
}

void VulkanSwapchain::init_Imageviews()
{

	std::uint32_t swapchainImageCount = 0;
	VkResult res = vkGetSwapchainImagesKHR(this->createInfo.logicalDevice, this->swapChain, &swapchainImageCount, nullptr);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to get swapchain images.");
	}

	std::vector<VkImage> tmpImageBuffer(swapchainImageCount);
	res = vkGetSwapchainImagesKHR(this->createInfo.logicalDevice, this->swapChain, &swapchainImageCount, tmpImageBuffer.data());
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to get swapchain images.");
	}

	for (VkImage tmpImage : tmpImageBuffer) {

		Image image = {};
		image.image = tmpImage;

		VkImageViewCreateInfo imageViewCreateInfo			= {};
		imageViewCreateInfo.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext							= NULL;
		imageViewCreateInfo.flags							= 0;
		imageViewCreateInfo.image							= image.image;
		imageViewCreateInfo.viewType						= VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format							= this->selectedSurfaceFormat.format;
		imageViewCreateInfo.components.r					= VK_COMPONENT_SWIZZLE_R;
		imageViewCreateInfo.components.g					= VK_COMPONENT_SWIZZLE_G;
		imageViewCreateInfo.components.b                    = VK_COMPONENT_SWIZZLE_B;
		imageViewCreateInfo.components.a                    = VK_COMPONENT_SWIZZLE_A;
		imageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
		imageViewCreateInfo.subresourceRange.levelCount     = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount     = 1;

		VkImageView view;
		res = vkCreateImageView(this->createInfo.logicalDevice, &imageViewCreateInfo, NULL, &view);
		if (res != VkResult::VK_SUCCESS) {
			throw std::runtime_error("Failed to create image view.");
		}

		image.imageView = view;
		this->imageCollection.push_back(image);
	}
}

void VulkanSwapchain::init_DepthTexture()
{
	this->depthTexture = VulkanTexture::NewDepthTexture(this->createInfo.device, this->createInfo.logicalDevice, this->selectedExtent.width, this->selectedExtent.height);
}

void VulkanSwapchain::init_Renderpass()
{
	// Attachment description
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = this->selectedSurfaceFormat.format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


	//Subpasses and attachment references
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	// -------------------------------------------

	// Depth attachement for depth buffering
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = VulkanUtils::FindDepthFormat(this->createInfo.device);
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;


	// -------------------------------------------


	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkResult res = vkCreateRenderPass(this->createInfo.logicalDevice, &renderPassInfo, nullptr, &this->renderPass);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to create renderpass");
	}
}

void VulkanSwapchain::init_Framebuffer()
{

	std::uint32_t swapchainImageCount = static_cast<std::uint32_t>(this->imageCollection.size());
	this->swapchainFramebufferCollection.resize(swapchainImageCount);

	for (size_t i = 0; i < swapchainImageCount; i++) {

		std::array<VkImageView, 2> attachments = {
			this->imageCollection.at(i).imageView,
			this->depthTexture->GetImageView()
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = this->renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = this->selectedExtent.width;
		framebufferInfo.height = this->selectedExtent.height;
		framebufferInfo.layers = 1;

		VkResult res = vkCreateFramebuffer(this->createInfo.logicalDevice, &framebufferInfo, nullptr, &this->swapchainFramebufferCollection[i]);
		if (res != VK_SUCCESS) {
			qDebug() << "Failed to create the framebuffer";
		}
	}
}

VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		VkExtent2D actualExtent = { this->createInfo.width, this->createInfo.height };

		actualExtent.width  = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

