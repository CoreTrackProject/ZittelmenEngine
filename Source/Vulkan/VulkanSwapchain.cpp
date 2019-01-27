#include "VulkanSwapchain.h"

VulkanSwapchain::VulkanSwapchain(VkPhysicalDevice *device, VkDevice* logicalDevice, DeviceInfo* deviceInfo, VkSurfaceKHR *surface)
{
	this->surface		= surface;
	this->device        = device;
	this->logicalDevice = logicalDevice;
	this->deviceInfo    = deviceInfo;

	this->init_Swapchain();
}

VulkanSwapchain::~VulkanSwapchain()
{
	vkDestroySwapchainKHR(*this->logicalDevice, this->swapChain, nullptr);
}

void VulkanSwapchain::init_Swapchain()
{
	this->querySwapChainRelatedInfo();

	VkSurfaceFormatKHR surfaceFormat = this->chooseSwapSurfaceFormat(this->details.formats);
	VkPresentModeKHR presentMode     = this->chooseSwapPresentMode(this->details.presentModes);
	VkExtent2D extent				 = this->chooseSwapExtent(this->details.capabilities);

	uint32_t imageCount = this->details.capabilities.minImageCount + 1;
	if (this->details.capabilities.maxImageCount > 0 && imageCount > this->details.capabilities.maxImageCount) {
		imageCount = this->details.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = *this->surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;

	/* 
		Link: https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
	*/
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Get queue family which supports presenting
	{

		uint32_t presentIdx = 0; // Queuefamily index which supports presenting
		{
			VkBool32 supprtPresent = VK_FALSE;
			for (uint32_t i = 0; i < this->deviceInfo->queueFamilyCount; i++) {
				vkGetPhysicalDeviceSurfaceSupportKHR(*this->device, i, *this->surface, &supprtPresent);
				if (supprtPresent == VK_TRUE) {
					presentIdx = i;
					break;
				}
			}
		}

		uint32_t queueFamilyIndices[] = { this->deviceInfo->queueFamilyIndexes.graphics, presentIdx };

		if (this->deviceInfo->queueFamilyIndexes.graphics != presentIdx) {
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
	createInfo.presentMode = presentMode;
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
		VkExtent2D actualExtent = { 800, 600 };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

