#include "VulkanSwapchain.h"

VulkanSwapchain::VulkanSwapchain(VkPhysicalDevice *device, VkDevice* logicalDevice, DeviceInfo* deviceInfo, VkSurfaceKHR *surface, uint32_t targetwidth, uint32_t targetheight)
{
	this->targetwidth   = targetwidth;
	this->targetheight  = targetheight;
	this->surface		= surface;
	this->device        = device;
	this->logicalDevice = logicalDevice;
	this->deviceInfo    = deviceInfo;

	this->init_Swapchain();
	this->init_Imageviews();
}

VulkanSwapchain::~VulkanSwapchain()
{

	for (Image image : this->imageCollection) {
		vkDestroyImageView(*this->logicalDevice, image.imageView, nullptr);
		//vkDestroyImage(*this->logicalDevice, image.image, nullptr);
	}

	vkDestroySwapchainKHR(*this->logicalDevice, this->swapChain, nullptr);

}

VkExtent2D* VulkanSwapchain::getSwapchainExtent2D()
{
	return &this->selectedExtent;
}

VkSurfaceFormatKHR* VulkanSwapchain::getSwapchainImageFormat()
{
	return &this->selectedSurfaceFormat;
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
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = *this->surface;

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
			for (uint32_t i = 0; i < this->deviceInfo->queueFamilyCount; i++) {
				vkGetPhysicalDeviceSurfaceSupportKHR(*this->device, i, *this->surface, &supprtPresent);
				if (supprtPresent == VK_TRUE) {
					this->queueFamilyPresentIdx = i;
					break;
				}
			}
		}

		uint32_t queueFamilyIndices[] = { this->deviceInfo->queueFamilyIndexes.graphics, this->queueFamilyPresentIdx };

		if (this->deviceInfo->queueFamilyIndexes.graphics != this->queueFamilyPresentIdx) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
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

	if (vkCreateSwapchainKHR(*this->logicalDevice, &createInfo, nullptr, &this->swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}


}

void VulkanSwapchain::querySwapChainRelatedInfo()
{

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*this->device, *this->surface, &details.capabilities);

	// Get surface formats
	{
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(*this->device, *this->surface, &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(*this->device, *this->surface, &formatCount, details.formats.data());
		}
	}
	
	// Get Presentmodes
	{
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(*this->device, *this->surface, &presentModeCount, nullptr);
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(*this->device, *this->surface, &presentModeCount, details.presentModes.data());
		}
		
	} 
}

void VulkanSwapchain::init_Imageviews()
{

	{
		uint32_t swapchainImageCount = 0;
		VkResult res = vkGetSwapchainImagesKHR(*this->logicalDevice, this->swapChain, &swapchainImageCount, nullptr);
		//this->imageCollection.resize(swapchainImageCount);

		std::vector<VkImage> tmpImageBuffer(swapchainImageCount);
		res = vkGetSwapchainImagesKHR(*this->logicalDevice, this->swapChain, &swapchainImageCount, tmpImageBuffer.data());

		for (VkImage tmpImage : tmpImageBuffer) {

			Image image;
			image.image = tmpImage;

			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.pNext							= NULL;
			imageViewCreateInfo.flags							= 0;
			imageViewCreateInfo.image							= image.image;
			imageViewCreateInfo.viewType						= VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format							= this->selectedSurfaceFormat.format;
			imageViewCreateInfo.components.r					= VK_COMPONENT_SWIZZLE_R;
			imageViewCreateInfo.components.g					= VK_COMPONENT_SWIZZLE_G;
			imageViewCreateInfo.components.b					= VK_COMPONENT_SWIZZLE_B;
			imageViewCreateInfo.components.a					= VK_COMPONENT_SWIZZLE_A;
			imageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
			imageViewCreateInfo.subresourceRange.levelCount     = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount     = 1;

			VkImageView view;
			res = vkCreateImageView(*this->logicalDevice, &imageViewCreateInfo, NULL, &view);
			image.imageView = view;


			this->imageCollection.push_back(image);
		}
	}


}

VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
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

VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { this->targetwidth, this->targetheight };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

std::vector<Image>* VulkanSwapchain::getImageCollection()
{
	return &this->imageCollection;
}

VkSwapchainKHR &VulkanSwapchain::getSwapchain()
{
	return this->swapChain;
}

uint32_t &VulkanSwapchain::getQueueFamilyPresentIdx()
{
	return this->queueFamilyPresentIdx;
}

